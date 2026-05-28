import sqlite3, os

def init_db():
  if os.path.exists("database.db"):
    return
  db = sqlite3.connect("database.db")
  db.executescript("""
    CREATE TABLE users (
      uid    TEXT PRIMARY KEY,
      name   TEXT NOT NULL,
      type   TEXT NOT NULL,
      access INTEGER DEFAULT 1
    );
    CREATE TABLE log (
      sl     INTEGER PRIMARY KEY AUTOINCREMENT,
      uid    TEXT NOT NULL,
      access INTEGER NOT NULL,
      time   DATETIME DEFAULT CURRENT_TIMESTAMP
    );
  """)
  db.close()
  print("database.db created")

if __name__ == "__main__":
  init_db()