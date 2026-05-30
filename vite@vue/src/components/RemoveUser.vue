<script setup>
  import { ref } from "vue"
  import { base } from "../base.js"
  import Popup from "./Popup.vue"
  defineProps(["visible"])
  const emit = defineEmits(["close", "done"])
  const step    = ref("form")
  const uid     = ref("")
  const message = ref("")
  const loading = ref(false)
  const found   = ref(null)
  const lookup = async () => {
    if (!/^[A-Z0-9]{8}$/.test(uid.value)) {
      message.value = "Enter a valid 8-character UID."
      return
    }
    message.value = ""
    try {
      const res  = await fetch(`${base.value}/api/users/lookup?uid=` + uid.value)
      const data = await res.json()
      if (data.success && data.user) {
        found.value   = data.user
        step.value    = "confirm"
      } else {
        message.value = "User not found."
      }
    } catch (e) { message.value = e.message }
  }
  const submit = async () => {
    loading.value = true
    try {
      const form = new FormData()
      form.append("uid", uid.value)
      const res  = await fetch(`${base.value}/api/users/remove`, { method: "POST", body: form })
      const data = await res.json()
      message.value = data.message
      if (data.success) { reset(); emit("done") }
    } catch (e) { message.value = e.message
    } finally { loading.value = false
    }
  }
  const reset = () => { step.value = "form"; uid.value = ""; message.value = ""; found.value = null }
  const close = () => { reset(); emit("close") }
</script>
<template>
  <Popup :visible="visible" @close="close">
    <p class="title">Remove User</p>
    <template v-if="step === 'form'">
      <p v-if="message" class="msg err">{{ message }}</p>
      <input v-model="uid" type="text" placeholder="UID (8 chars)"
             maxlength="8" @input="uid = uid.toUpperCase()" @keyup.enter="lookup" />
      <div class="row">
        <button class="btn-ok" @click="lookup">Find</button>
        <button class="btn-no" @click="close">Cancel</button>
      </div>
    </template>
    <template v-else>
      <p class="confirm-text">Remove <strong>{{ found?.name }}</strong> ({{ found?.type }})?</p>
      <p v-if="message" class="msg err">{{ message }}</p>
      <div class="row">
        <button class="btn-ok" :disabled="loading" @click="submit">{{ loading ? "Removing…" : "Yes, Remove" }}</button>
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
    padding: 8px; flex: 1;
    border-radius: 8px;
    font-weight: bold;
    cursor: pointer;
  }
  .msg {
    font-size: 0.85rem;
    text-align: center;
  }
  .confirm-text {
    text-align: center;
    font-size: 0.95rem;
  }
  .btn-ok { background: var(--g); }
  .btn-no { background: var(--r); }
  .err { color: var(--r); }
</style>