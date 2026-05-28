<script setup>
  import { ref } from "vue"
  import { base } from "../base.js"
  
  const isLoading = ref(false)
  const username  = ref("")
  const password  = ref("")
  const message   = ref("")
  const shake     = ref(false)
  const emit      = defineEmits(["loggedIn"])
  
  const handleLogin = async () => {
    isLoading.value = true
    message.value   = ""
    try {
      const form = new FormData()
      form.append("username", username.value)
      form.append("password", password.value)
      const res = await fetch(`${base.value}/api/auth/login`, { method: "POST", body: form, credentials: "include" })
      const data = await res.json()
      if (data.success) emit("loggedIn")
      else {
        message.value = data.message
        shake.value = true
        setTimeout(() => { shake.value = false }, 500)
      }
    } catch (err) {
      message.value = err.message
      shake.value = true
      setTimeout(() => { shake.value = false }, 500)
    } finally { isLoading.value = false
    }
  }
</script>

<template>
  <div class="root">
    <div class="card" :class="{ shake }">
      <div class="header">
        <div class="badge">RFID</div>
        <h1>Admin Panel</h1>
        <p>Authorized personnel only</p>
      </div>
      <form @submit.prevent="handleLogin">
        <div class="field">
          <label for="usr">Username</label>
          <input id="usr" v-model="username" type="text"
            autocomplete="username" spellcheck="false"
            placeholder="enter username" required />
        </div>
        <div class="field">
          <label for="pwd">Password</label>
          <input id="pwd" v-model="password" type="password"
            autocomplete="current-password"
            placeholder="enter password" required />
        </div>
        <Transition name="err">
          <p v-if="message" class="error">{{ message }}</p>
        </Transition>
        <button type="submit" :disabled="isLoading">
          <span v-if="isLoading" class="dots"><i/><i/><i/></span>
          <span v-else>Login</span>
        </button>
      </form>
    </div>
  </div>
</template>

<style scoped>
  @import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&display=swap');
  .root {
    font-family: 'Share Tech Mono', monospace;
    background: var(--background);
    justify-content: center;
    align-items: center;
    position: fixed;
    display: flex;
    inset: 0;
  }
  .root::before {
    background-image: radial-gradient(circle, rgba(125,250,125,0.08) 1px, transparent 1px);
    background-size: 24px 24px;
    position: absolute;
    pointer-events: none;
    content: '';
    inset: 0;
  }
  .card {
    animation: rise 0.4s cubic-bezier(0.34,1.4,0.64,1) both;
    box-shadow: 0 0 40px rgba(125,250,125,0.06);
    border: 1px solid rgba(125,250,125,0.3);
    background: var(--background);
    padding: 36px 28px 28px;
    width: min(88vw, 360px);
    border-radius: 6px;
    position: relative;
    z-index: 1;
  }
  .card::before {
    background: rgba(125,250,125,0.7);
    border-radius: 0 0 4px 4px;
    position: absolute;
    content: '';
    height: 2px;
    right: 10%;
    left: 10%;
    top: -1px;
  }
  @keyframes rise {
    from {
      transform: translateY(20px);
      opacity: 0;
    }
    to {
      transform: none;
      opacity: 1;
    }
  }
  @keyframes shake {
    0%,100% { transform: translateX(0); }
    20%,60% { transform: translateX(-6px); }
    40%,80% { transform: translateX(6px); }
  }
  .shake { animation: shake 0.4s ease both; }
  .header {
    margin-bottom: 28px;
    text-align: center;
  }
  .badge {
    border: 1px solid rgba(125,250,125,0.5);
    background: rgba(125,250,125,0.06);
    color: rgba(125,250,125,0.9);
    display: inline-block;
    letter-spacing: 3px;
    margin-bottom: 12px;
    border-radius: 2px;
    font-size: 0.65rem;
    padding: 3px 10px;
  }
  h1 {
    color: var(--foreground);
    background: transparent;
    letter-spacing: 2px;
    margin-bottom: 4px;
    font-size: 1.6rem;
    font-weight: 400;
  }
  .header p {
    background: transparent;
    letter-spacing: 1px;
    font-size: 0.7rem;
    opacity: 0.35;
  }
  .field { margin-bottom: 18px; }
  label {
    color: rgba(125,250,125,0.6);
    text-transform: uppercase;
    background: transparent;
    letter-spacing: 2px;
    margin-bottom: 6px;
    font-size: 0.68rem;
    display: block;
  }
  input {
    transition: border-color 0.2s, background 0.2s;
    font-family: 'Share Tech Mono', monospace;
    border: 1px solid rgba(125,250,125,0.2);
    background: rgba(125,250,125,0.04);
    color: var(--foreground);
    border-radius: 4px;
    padding: 10px 12px;
    font-size: 0.9rem;
    outline: none;
    width: 100%;
  }
  input::placeholder { color: rgba(125,250,125,0.2); }
  input:focus {
    border-color: rgba(125,250,125,0.55);
    background: rgba(125,250,125,0.07);
  }
  .error {
    border: 1px solid rgba(250,125,125,0.25);
    background: rgba(250,125,125,0.07);
    color: rgba(250,125,125,0.9);
    letter-spacing: 0.5px;
    margin-bottom: 14px;
    font-size: 0.78rem;
    border-radius: 4px;
    padding: 8px 12px;
  }
  .err-enter-active { transition: opacity 0.2s, transform 0.2s; }
  .err-enter-from { opacity: 0; transform: translateY(-4px); }
  button {
    background: rgba(125,250,125,0.12) !important;
    transition: background 0.2s, box-shadow 0.2s;
    font-family: 'Share Tech Mono', monospace;
    border: 1px solid rgba(125,250,125,0.45);
    color: rgba(125,250,125,0.95) !important;
    fill: rgba(125,250,125,0.95) !important;
    letter-spacing: 3px;
    border-radius: 4px;
    font-size: 0.85rem;
    cursor: pointer;
    margin-top: 4px;
    padding: 11px;
    width: 100%;
  }
  button:hover:not(:disabled) {
    background: rgba(125,250,125,0.2) !important;
    box-shadow: 0 0 16px rgba(125,250,125,0.12);
  }
  button:disabled {
    cursor: not-allowed;
    opacity: 0.5;
  }
  .dots {
    display: inline-flex;
    align-items: center;
    gap: 5px;
  }
  .dots i {
    background: rgba(125,250,125,0.8) !important;
    animation: dp 1s ease-in-out infinite;
    display: inline-block;
    border-radius: 50%;
    height: 5px;
    width: 5px;
  }
  .dots i:nth-child(2) { animation-delay: 0.18s; }
  .dots i:nth-child(3) { animation-delay: 0.36s; }
  @keyframes dp {
    0%,80%,100% {
      transform: scale(0.6);
      opacity: 0.3;
    }
    40% {
      transform: scale(1);
      opacity: 1;
    }
  }
</style>