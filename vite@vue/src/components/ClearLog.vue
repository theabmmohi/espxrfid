<script setup>
  import { ref } from "vue"
  import { base } from "../base.js"
  import Popup from "./Popup.vue"
  
  defineProps(["visible"])
  const emit = defineEmits(["close", "done"])
  const input   = ref("")
  const message = ref("")
  const loading = ref(false)
  const step    = ref("form")
  const toConfirm = () => {
    if (input.value !== "Clear LOG") { message.value = 'Type exactly: Clear LOG'; return }
    step.value = "confirm"
  }
  const submit = async () => {
    loading.value = true
    try {
      const res  = await fetch(`${base.value}/api/log/clear`, { method: "POST" })
      const data = await res.json()
      if (data.success) { reset(); emit("done") }
      else message.value = data.message
    } catch (e) { message.value = e.message
    } finally { loading.value = false
    }
  }
  const reset = () => { input.value = ""; message.value = ""; step.value = "form" }
  const close = () => { reset(); emit("close") }
</script>
<template>
  <Popup :visible="visible" @close="close">
    <p class="title">Clear Log</p>
    <template v-if="step === 'form'">
      <p class="hint">Type <code>Clear LOG</code> to continue</p>
      <p v-if="message" class="msg err">{{ message }}</p>
      <input v-model="input" type="text" placeholder='Type "Clear LOG"' @keyup.enter="toConfirm" />
      <div class="row">
        <button class="btn-ok" :disabled="input !== 'Clear LOG'" @click="toConfirm">Next</button>
        <button class="btn-no" @click="close">Cancel</button>
      </div>
    </template>
    <template v-else>
      <p class="confirm-text">This will <strong>permanently delete</strong> all log entries. Are you sure?</p>
      <p v-if="message" class="msg err">{{ message }}</p>
      <div class="row">
        <button class="btn-ok" :disabled="loading" @click="submit">{{ loading ? "Clearing…" : "Yes, Clear" }}</button>
        <button class="btn-no" @click="step = 'form'">Back</button>
      </div>
    </template>
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
  button:disabled {
    cursor: not-allowed;
    opacity: 0.45;
  }
  .msg {
    text-align: center;
    font-size: 0.85rem;
  }
  .confirm-text {
    text-align: center;
    font-size: 0.95rem;
  }
  code {
    background: rgba(125,125,125,0.2);
    border-radius: 4px;
    padding: 1px 5px;
  }
  .btn-ok { background: var(--g); }
  .btn-no { background: var(--r); }
  .err  { color: var(--r); }
</style>