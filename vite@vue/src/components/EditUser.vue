<script setup>
  import { ref } from "vue"
  import { base } from "../base.js"
  import Popup from "./Popup.vue"
  
  defineProps(["visible"])
  const emit = defineEmits(["close", "done"])
  
  const step    = ref("search")
  const uid     = ref("")
  const name    = ref("")
  const type    = ref("")
  const message = ref("")
  const loading = ref(false)
  
  const lookup = async () => {
    if (!/^[A-Z0-9]{8}$/.test(uid.value)) {
      message.value = "Enter a valid 8-character UID."
      return
    }
    message.value = ""
    try {
      const res  = await fetch(`${base.value}/api/users/lookup?uid=` + uid.value, { credentials: "include" })
      const data = await res.json()
      if (data.success && data.user) {
        name.value = data.user.name
        type.value = data.user.type
        step.value = "edit"
      } else {
        message.value = "User not found."
      }
    } catch (e) { message.value = e.message }
  }
  const submit = async () => {
    loading.value = true
    try {
      const form = new FormData()
      form.append("uid",  uid.value)
      form.append("name", name.value.trim())
      form.append("type", type.value.trim())
      const res  = await fetch(`${base.value}/api/users/edit`, { method: "POST", body: form, credentials: "include" })
      const data = await res.json()
      message.value = data.message
      if (data.success) { reset(); emit("done") }
    } catch (e) { message.value = e.message
    } finally { loading.value = false
    }
  }
  const reset = () => { step.value = "search"; uid.value = name.value = type.value = message.value = "" }
  const close = () => { reset(); emit("close") }
</script>
<template>
  <Popup :visible="visible" @close="close">
    <p class="title">Edit User</p>
    <template v-if="step === 'search'">
      <p v-if="message" class="msg err">{{ message }}</p>
      <input v-model="uid" type="text" placeholder="UID (8 chars)"
             maxlength="8" @input="uid = uid.toUpperCase()" @keyup.enter="lookup" />
      <div class="row">
        <button class="btn-ok" @click="lookup">Find</button>
        <button class="btn-no" @click="close">Cancel</button>
      </div>
    </template>
    <template v-else-if="step === 'edit'">
      <p v-if="message" class="msg err">{{ message }}</p>
      <input :value="uid" readonly class="readonly" />
      <input v-model="name" type="text" placeholder="New Name" @keyup.enter="submit" />
      <input v-model="type" type="text" placeholder="New Type" @keyup.enter="submit" />
      <div class="row">
        <button class="btn-ok" :disabled="loading" @click="submit">{{ loading ? "Saving…" : "Save" }}</button>
        <button class="btn-no" @click="step = 'search'">Back</button>
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
  .readonly {
    cursor: not-allowed;
    opacity: 0.5;
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
    font-size: 0.85rem;
    text-align: center;
  }
  .btn-ok { background: var(--g); }
  .btn-no { background: var(--r); }
  .err  { color: var(--r); }
</style>