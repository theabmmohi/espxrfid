import sqlite3, hashlib, base64, struct, time, hmac, os, json, queue, threading
from   flask      import Flask, render_template, request, jsonify, session, Response, stream_with_context
from   dotenv     import load_dotenv
from   flask_cors import CORS
from   init_db    import init_db
load_dotenv()
init_db()
TZ_OFFS = int(os.getenv("TZ_OFFS", 6))
app = Flask(__name__)
app.secret_key = os.getenv("FLASK_K")
app.config["SESSION_COOKIE_SAMESITE"] = "None"
app.config["SESSION_COOKIE_SECURE"] = True
CORS(app, supports_credentials=True)
_subscribers      = []
_subscribers_lock = threading.Lock()
def _push(data: dict):
  payload = f"data: {json.dumps(data)}\n\n"
  with _subscribers_lock:
    for q in list(_subscribers):
      try:
        q.put_nowait(payload)
      except queue.Full:
        pass
def _fmt_time() -> str:
  months = ["Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"]
  t      = time.gmtime(time.time() + TZ_OFFS * 3600)
  hour12 = t.tm_hour % 12 or 12
  ampm   = "AM" if t.tm_hour < 12 else "PM"
  return f"{hour12:02d}:{t.tm_min:02d}:{t.tm_sec:02d} {ampm}\n{t.tm_mday:02d} {months[t.tm_mon-1]} {t.tm_year}"
def connectDB():
  connection = sqlite3.connect("database.db", timeout=10)
  connection.execute("PRAGMA journal_mode=WAL")
  connection.row_factory = sqlite3.Row
  return connection
def getTOTP(offset=0):
  secret  = os.getenv("OTP_SEC")
  counter = struct.pack(">Q", int(time.time() / 30) + offset)
  key     = base64.b32decode(secret.upper() + "=" * (len(secret) % 8))
  haash   = hmac.new(key, counter, hashlib.sha1).digest()
  offset  = haash[19] & 0xf
  code    = (struct.unpack(">I", haash[offset:offset+4])[0] & 0x7fffffff) % 1000000
  return str(code).zfill(6)
def requireLogin():
  if not session.get("isLoggedIn", False):
    return jsonify({"success": False, "message": "Not logged in"}), 401
  return None
@app.route("/")
def index():
  return render_template("index.html")
@app.route("/help")
def help_page():
  ino_path = os.path.join(app.static_folder, "main.ino")
  ino_content = ""
  if os.path.exists(ino_path):
    with open(ino_path, "r") as f:
      ino_content = f.read()
  return render_template("help.html", ino_content=ino_content)
@app.route("/api/esp")
def api():
  uid = request.args.get("uid", "").upper().strip()
  key = request.args.get("key", "").strip()
  if not key or not uid or len(uid) != 8:
    return "", 400
  auth = False
  if key == getTOTP() or key == getTOTP(-1):
    auth = True
  if not auth:
    return "", 401
  name, typE, accs = "Unknown", "Unknown", 0
  sl = None
  try:
    db = connectDB()
    cursor = db.cursor()
    cursor.execute("SELECT name, type, access FROM users WHERE uid = ? LIMIT 1", (uid,))
    user = cursor.fetchone()
    if user:
      name, typE, accs = user["name"], user["type"], int(user["access"])
    cursor.execute("INSERT INTO log (uid, access) VALUES (?, ?)", (uid, accs))
    db.commit()
    sl = cursor.lastrowid
    db.close()
  except Exception as error:
    return str(error), 500
  _push({
    "sl":     sl,
    "name":   name,
    "type":   typE,
    "uid":    uid,
    "access": accs,
    "time":   _fmt_time()
  })
  return f"{accs}|{name}|{typE}", 200, {"content-type": "text/plain"}
@app.route("/api/events")
def events():
  if not session.get("isLoggedIn", False):
    return "", 401
  def stream():
    q = queue.Queue(maxsize=50)
    with _subscribers_lock:
      _subscribers.append(q)
    try:
      yield ": connected\n\n"
      while True:
        try:
          data = q.get(timeout=25)
          yield data
        except queue.Empty:
          yield ": ping\n\n"
    except GeneratorExit:
      pass
    finally:
      with _subscribers_lock:
        if q in _subscribers:
          _subscribers.remove(q)
  return Response(
    stream_with_context(stream()),
    mimetype="text/event-stream",
    headers={
      "Cache-Control":     "no-cache",
      "X-Accel-Buffering": "no",
      "Connection":        "keep-alive",
    }
  )
@app.route("/api/auth/login", methods=["POST"])
def login():
  username   = request.form.get("username", "")
  password   = request.form.get("password", "")
  authorized = (username == os.getenv("APP_USR") and password == os.getenv("APP_PWD"))
  if authorized:
    session.permanent = True
    session["isLoggedIn"] = True
  else:
    session["isLoggedIn"] = False
  logged_in = session.get("isLoggedIn")
  return jsonify({"success": logged_in, "message": "Logged In" if logged_in else "Wrong Credentials"})
@app.route("/api/auth/logout")
def logout():
  session.clear()
  return jsonify({"success": True, "isLoggedIn": session.get("isLoggedIn", False)})
@app.route("/api/auth/isLoggedIn")
def isLoggedIn():
  return jsonify({"success": True, "loggedIn": session.get("isLoggedIn", False)})
@app.route("/api/view/log")
def viewLog():
  err = requireLogin()
  if err:
    return err
  db = connectDB()
  cursor = db.cursor()
  tz = f"{TZ_OFFS:+d} hours"
  cursor.execute(f"""
    SELECT
      l.sl,
      IFNULL(u.name, 'Unknown') AS name,
      IFNULL(u.type, 'Unknown') AS type,
      (
      CASE
        WHEN cast(strftime('%H', datetime(l.time, '{tz}')) as int) % 12 = 0 THEN '12'
        ELSE printf('%02d', cast(strftime('%H', datetime(l.time, '{tz}')) as int) % 12)
      END || ':' ||
      strftime('%M:%S', datetime(l.time, '{tz}')) || ' ' ||
      CASE WHEN cast(strftime('%H', datetime(l.time, '{tz}')) as int) < 12 THEN 'AM' ELSE 'PM' END ||
      '\n' ||
      strftime('%d ', datetime(l.time, '{tz}')) ||
      CASE strftime('%m', datetime(l.time, '{tz}'))
        WHEN '01' THEN 'Jan' WHEN '02' THEN 'Feb' WHEN '03' THEN 'Mar'
        WHEN '04' THEN 'Apr' WHEN '05' THEN 'May' WHEN '06' THEN 'Jun'
        WHEN '07' THEN 'Jul' WHEN '08' THEN 'Aug' WHEN '09' THEN 'Sep'
        WHEN '10' THEN 'Oct' WHEN '11' THEN 'Nov' WHEN '12' THEN 'Dec'
      END ||
      strftime(' %Y', datetime(l.time, '{tz}'))) AS time,
      l.uid,
      l.access
    FROM log l
    LEFT JOIN users u ON l.uid = u.uid
    ORDER BY l.sl DESC
  """)
  rows = cursor.fetchall()
  db.close()
  return jsonify({"success": True, "data": [dict(row) for row in rows]})
@app.route("/api/view/users")
def viewUsers():
  err = requireLogin()
  if err:
    return err
  db = connectDB()
  cursor = db.cursor()
  cursor.execute("SELECT * FROM users ORDER BY name")
  rows = cursor.fetchall()
  db.close()
  return jsonify({"success": True, "data": [dict(row) for row in rows]})
@app.route("/api/users/lookup")
def lookupUser():
  err = requireLogin()
  if err:
    return err
  uid = request.args.get("uid", "").upper().strip()
  if len(uid) != 8:
    return jsonify({"success": False, "message": "Invalid UID"}), 400
  db = connectDB()
  cursor = db.cursor()
  cursor.execute("SELECT uid, name, type, access FROM users WHERE uid = ? LIMIT 1", (uid,))
  user = cursor.fetchone()
  db.close()
  if user:
    return jsonify({"success": True, "user": dict(user)})
  return jsonify({"success": False, "message": "User not found"})
@app.route("/api/users/add", methods=["POST"])
def addUser():
  err = requireLogin()
  if err:
    return err
  name  = request.form.get("name", "").strip()
  typeV = request.form.get("type", "").strip()
  uid   = request.form.get("uid",  "").upper().strip()
  if not name or not typeV or len(uid) != 8:
    return jsonify({"success": False, "message": "Invalid data (UID must be 8 chars)"}), 400
  try:
    db = connectDB()
    cursor = db.cursor()
    cursor.execute("INSERT INTO users (uid, name, type, access) VALUES (?, ?, ?, 1)", (uid, name, typeV))
    db.commit()
    db.close()
    return jsonify({"success": True, "message": f"{name} added successfully"})
  except Exception as e:
    return jsonify({"success": False, "message": str(e)}), 500
@app.route("/api/users/remove", methods=["POST"])
def removeUser():
  err = requireLogin()
  if err:
    return err
  uid = request.form.get("uid", "").upper().strip()
  if len(uid) != 8:
    return jsonify({"success": False, "message": "Invalid UID"}), 400
  try:
    db = connectDB()
    cursor = db.cursor()
    cursor.execute("DELETE FROM users WHERE uid = ?", (uid,))
    db.commit()
    db.close()
    return jsonify({"success": True, "message": "User removed"})
  except Exception as e:
    return jsonify({"success": False, "message": str(e)}), 500
@app.route("/api/users/edit", methods=["POST"])
def editUser():
  err = requireLogin()
  if err:
    return err
  uid   = request.form.get("uid",  "").upper().strip()
  name  = request.form.get("name", "").strip()
  typeV = request.form.get("type", "").strip()
  if len(uid) != 8:
    return jsonify({"success": False, "message": "Invalid UID"}), 400
  updates, values = [], []
  if name:
    updates.append("name = ?")
    values.append(name)
  if typeV:
    updates.append("type = ?")
    values.append(typeV)
  if not updates:
    return jsonify({"success": False, "message": "Nothing to update"}), 400
  values.append(uid)
  try:
    db = connectDB()
    cursor = db.cursor()
    cursor.execute(f"UPDATE users SET {', '.join(updates)} WHERE uid = ?", values)
    db.commit()
    db.close()
    return jsonify({"success": True, "message": "User updated"})
  except Exception as e:
    return jsonify({"success": False, "message": str(e)}), 500
@app.route("/api/users/toggle", methods=["POST"])
def toggleAccess():
  err = requireLogin()
  if err:
    return err
  uid    = request.form.get("uid",    "").upper().strip()
  access = request.form.get("access", "0")
  if len(uid) != 8:
    return jsonify({"success": False, "message": "Invalid UID"}), 400
  try:
    db = connectDB()
    cursor = db.cursor()
    cursor.execute("UPDATE users SET access = ? WHERE uid = ?", (int(access), uid))
    db.commit()
    db.close()
    return jsonify({"success": True})
  except Exception as e:
    return jsonify({"success": False, "message": str(e)}), 500
@app.route("/api/log/clear", methods=["POST"])
def clearLog():
  err = requireLogin()
  if err:
    return err
  try:
    db = connectDB()
    cursor = db.cursor()
    cursor.execute("DELETE FROM log")
    db.commit()
    db.close()
    return jsonify({"success": True, "message": "Log cleared"})
  except Exception as e:
    return jsonify({"success": False, "message": str(e)}), 500
@app.errorhandler(400)
@app.errorhandler(401)
@app.errorhandler(403)
@app.errorhandler(404)
@app.errorhandler(500)
@app.errorhandler(503)
def handleError(error):
  return render_template("error.html", error=error), error.code
if __name__ == "__main__":
  app.run(host="0.0.0.0", port=8000, threaded=True)