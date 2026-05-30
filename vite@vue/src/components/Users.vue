<script setup>
  import { ref, onMounted } from "vue"
  import { base } from "../base.js"
  
  const users      = ref([])
  const toggling   = ref(null)
  const toggleErr  = ref("")
  
  const getUsers = async () => {
    const response = await fetch(`${base.value}/api/view/users`)
    const res = await response.json()
    if (res.success) users.value = res.data
  }
  
  const toggleAccess = async (row) => {
    toggling.value  = row.uid
    toggleErr.value = ""
    const newAccess = row.access ? 0 : 1
    try {
      const form = new FormData()
      form.append("uid",    row.uid)
      form.append("access", newAccess)
      const res  = await fetch(`${base.value}/api/users/toggle`, { method: "POST", body: form })
      const data = await res.json()
      if (data.success) {
        row.access = newAccess
      } else {
        toggleErr.value = data.message || "Toggle failed"
        setTimeout(() => { toggleErr.value = "" }, 2500)
      }
    } catch (e) {
      toggleErr.value = e.message
      setTimeout(() => { toggleErr.value = "" }, 2500)
    } finally {
      toggling.value = null
    }
  }
  
  const copyUID = (uid) => navigator.clipboard?.writeText(uid)
  
  onMounted(getUsers)
</script>
<template>
  <Teleport to="body">
    <div v-if="toggling" class="toggle-overlay">
      <div class="spinner"></div>
      <p v-if="toggleErr" class="err-msg">{{ toggleErr }}</p>
    </div>
  </Teleport>
  <div class="table-wrap">
    <table class="users-table">
      <thead>
        <tr>
          <th>Name</th><th>Type</th>
          <th title="Click to copy">UID</th><th>Access</th>
        </tr>
      </thead>
      <tbody>
        <tr v-for="row in users" :key="row.uid" :class="row.access ? 'row-yes' : 'row-no'">
          <td>{{ row.name }}</td>
          <td>{{ row.type }}</td>
          <td class="uid-cell" @click="copyUID(row.uid)" title="Click to copy">{{ row.uid }}</td>
          <td class="toggle-cell">
            <input
              type="checkbox" :id="'chk-' + row.uid"
              :checked="!!row.access"
              :disabled="!!toggling"
              @change="toggleAccess(row)"
            />
            <label :for="'chk-' + row.uid">
              <span :class="row.access ? 'thumb-yes' : 'thumb-no'"></span>
            </label>
          </td>
        </tr>
        <tr v-if="!users.length">
          <td colspan="4" class="empty">No users found.</td>
        </tr>
      </tbody>
    </table>
  </div>
</template>
<style scoped>
  .table-wrap {
    overflow: auto;
    padding: 16px;
    height: 100dvh;
  }
  .users-table {
    border-collapse: collapse;
    min-width: 100%;
  }
  .users-table, th, td { border: 1px solid var(--foreground); }
  th {
    background: var(--background);
    padding: 4px 8px;
  }
  td {
    transition: background 200ms;
    padding: 6px 8px;
  }
  .uid-cell {
  font-family: monospace;
  text-align: center;
  cursor: pointer;
  }
  label {
    border: 1.25px solid var(--foreground);
    background: var(--background);
    vertical-align: middle;
    display: inline-block;
    border-radius: 50px;
    position: relative;
    cursor: pointer;
    height: 24px;
    width: 48px;
  }
  input:disabled + label {
    cursor: wait;
    opacity: 0.6;
  }
  label span {
    transition: background 250ms, transform 250ms;
    border: 0.25px solid var(--foreground);
    border-radius: 50px;
    position: absolute;
    display: block;
    height: 100%;
    width: 50%;
  }
  .empty {
    color: var(--foreground);
    text-align: center;
    padding: 20px;
    opacity: 0.5;
  }
  .toggle-overlay {
    background: rgba(0,0,0,0.45);
    backdrop-filter: blur(3px);
    justify-content: center;
    flex-direction: column;
    align-items: center;
    position: fixed;
    display: flex;
    z-index: 200;
    gap: 16px;
    inset: 0;
  }
  .spinner {
    border: 4px solid rgba(255,255,255,0.25);
    animation: spin 0.7s linear infinite;
    border-top-color: var(--foreground);
    border-radius: 50%;
    height: 44px;
    width: 44px;
  }
  .err-msg {
    background: var(--r);
    border-radius: 8px;
    padding: 8px 16px;
    font-weight: bold;
  }
  
  @keyframes spin {
    to {
      transform: rotate(360deg);
    }
  }
  
  .thumb-yes { background: var(--g); transform: translateX(100%); }
  .thumb-no  { background: var(--r); transform: translateX(000%); }
  input[type="checkbox"] { display: none; }
  .toggle-cell { text-align: center; }
  .row-yes { background: var(--g); }
  .row-no  { background: var(--r); }
  .uid-cell:hover { opacity: 0.7; }
</style>