<script setup>
  import { ref, onMounted } from "vue"
  import { base } from "./base.js"
  import Dashboard from "./components/Dashboard.vue"
  import LoginPage from "./components/LoginPage.vue"

  const isLoggedIn  = ref(false)
  const isChecking  = ref(true)

  const checkStatus = async () => {
    const response  = await fetch(`${base.value}/api/auth/isLoggedIn`, { credentials: "include" })
    const res       = await response.json()
    isLoggedIn.value = res.loggedIn
    isChecking.value = false
  }

  onMounted(checkStatus)
</script>
<template>
  <div v-if="isChecking"></div>
  <div v-else>
    <LoginPage v-if="!isLoggedIn" @loggedIn="isLoggedIn = true" />
    <Dashboard v-else @loggedOut="isLoggedIn = false" />
  </div>
</template>
