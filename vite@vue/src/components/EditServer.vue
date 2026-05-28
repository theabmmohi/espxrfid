<script setup>
  import { ref } from "vue"
  import Popup from "./Popup.vue"
  import { base } from "../base.js"

  defineProps(["visible"])
  const emit = defineEmits(["close"])

  const defaultBase = `${window.location.protocol}//${window.location.host}`
  const input   = ref(base.value)
  const message = ref("")
  const saved   = ref(false)

  const save = () => {
    const val = input.value.trim()
    if (!val) { message.value = "Server URL cannot be empty."; return }
    try { new URL(val) } catch { message.value = "Enter a valid URL (e.g. http://192.168.1.1)"; return }
    base.value = val
    localStorage.setItem("serverBase", val)
    message.value = ""
    saved.value = true
    setTimeout(() => { saved.value = false }, 2000)
  }

  const reset = () => {
    input.value = defaultBase
    base.value  = defaultBase
    localStorage.removeItem("serverBase")
    message.value = ""
    saved.value = true
    setTimeout(() => { saved.value = false }, 2000)
  }

  const close = () => { message.value = ""; emit("close") }
</script>

<template>
  <Popup :visible="visible" @close="close">
    <p class="title">Edit Server</p>
    <p class="hint">Set the base URL for API calls</p>

    <p v-if="message" class="msg err">{{ message }}</p>
    <p v-if="saved"   class="msg ok">Saved!</p>

    <input v-model="input" type="text" placeholder="http://192.168.1.1" @keyup.enter="save" />

    <p class="current">Current: <code>{{ base }}</code></p>

    <div class="row">
      <button class="btn-ok" @click="save">Save</button>
      <button class="btn-reset" @click="reset">Reset</button>
      <button class="btn-no" @click="close">Close</button>
    </div>
  </Popup>
</template>

<style scoped>
  .title {
    text-align: center;
    font-weight: bold;
    font-size: 1.1rem;
  }
  .hint {
    text-align: center;
    font-size: 0.85rem;
    opacity: 0.7;
  }
  .current {
    font-size: 0.8rem;
    text-align: center;
    opacity: 0.6;
  }
  input {
    border: 1px solid var(--foreground);
    border-radius: 8px;
    font-size: 0.95rem;
    padding: 8px 10px;
    width: 100%;
  }
  .row {
    display: flex;
    gap: 10px;
  }
  button {
    border-radius: 8px;
    font-weight: bold;
    cursor: pointer;
    padding: 8px;
    flex: 1;
  }
  .msg {
    text-align: center;
    font-size: 0.85rem;
  }
  code {
    background: rgba(125,125,125,0.2);
    border-radius: 4px;
    padding: 1px 5px;
    word-break: break-all;
  }
  .btn-ok    { background: var(--g); }
  .btn-no    { background: var(--r); }
  .btn-reset { background: var(--foreground); color: var(--background); }
  .err { color: var(--r); }
  .ok  { color: var(--g); }
</style>
