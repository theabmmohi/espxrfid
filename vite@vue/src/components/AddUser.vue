<script setup>
  import { ref } from "vue"
  import { base } from "../base.js"
  import Popup from "./Popup.vue"
  
  defineProps(["visible"])
  const emit = defineEmits(["close", "done"])
  const step    = ref("form")
  const name    = ref("")
  const type    = ref("")
  const uid     = ref("")
  const message = ref("")
  const loading = ref(false)
  const toConfirm = () => {
    if (!name.value.trim() || !type.value.trim() || !/^[A-Z0-9]{8}$/.test(uid.value)) {
      message.value = "Fill all fields correctly (UID: 8 uppercase chars/digits)."
      return
    }
    message.value = ""
    step.value = "confirm"
  }
  const submit = async () => {
    loading.value = true
    try {
      const form = new FormData()
      form.append("name", name.value.trim())
      form.append("type", type.value.trim())
      form.append("uid",  uid.value.trim())
      const res  = await fetch(`${base.value}/api/users/add`, { method: "POST", body: form })
      const data = await res.json()
      message.value = data.message
      if (data.success) {
        reset()
        emit("done")
      }
    } catch (e) { message.value = e.message }
    finally { loading.value = false }
  }
  const reset = () => {
    step.value = "form"
    name.value = type.value = uid.value = message.value = ""
  }
  const close = () => { reset(); emit("close") }
</script>
<template>
  <Popup :visible="visible" @close="close">
    <p class="title">Add User</p>
    
    <template v-if="step === 'form'">
      <p v-if="message" class="msg err">{{ message }}</p>
      <input v-model="name" type="text"  placeholder="Full Name"        @keyup.enter="toConfirm" />
      <input v-model="type" type="text"  placeholder="Type (e.g. Staff)" @keyup.enter="toConfirm" />
      <input v-model="uid"  type="text"  placeholder="UID (8 chars)"
             maxlength="8" @input="uid = uid.toUpperCase()" @keyup.enter="toConfirm" />
      <div class="row">
        <button class="btn-ok"  @click="toConfirm">Next</button>
        <button class="btn-no"  @click="close">Cancel</button>
      </div>
    </template>
    
    <template v-else>
      <p class="confirm-text">Add <strong>{{ name }}</strong> ({{ type }}) with UID <code>{{ uid }}</code>?</p>
      <p v-if="message" class="msg err">{{ message }}</p>
      <div class="row">
        <button class="btn-ok"  :disabled="loading" @click="submit">{{ loading ? "Adding…" : "Yes, Add" }}</button>
        <button class="btn-no"  @click="step = 'form'">Back</button>
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
  .err { color: var(--r); }
</style>