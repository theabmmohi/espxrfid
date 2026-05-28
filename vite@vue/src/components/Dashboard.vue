<script setup>
  import { ref, computed, watch, onMounted } from "vue"
  import { base } from "../base.js"
  import { useStorage } from "@vueuse/core"
  import VueDraggableResizable from "vue-draggable-resizable"
  import EditServer from "./EditServer.vue"
  import RemoveUser from "./RemoveUser.vue"
  import EditUser   from "./EditUser.vue"
  import ClearLog   from "./ClearLog.vue"
  import AddUser    from "./AddUser.vue"
  import Users      from "./Users.vue"
  import Log        from "./Log.vue"
  
  const posi      = useStorage("pos", { x: 100, y: -500 })
  const currView  = useStorage("currView", "log")
  const themeIdx  = useStorage("themeIdx", 0)
  const themes    = ["system", "light", "dark"]
  
  const emit       = defineEmits(["loggedOut"])
  const menuVsbl   = ref(false)
  const settVsbl   = ref(false)
  const panelState = ref("none")
  
  const addOpen    = ref(false)
  const removeOpen = ref(false)
  const editOpen   = ref(false)
  const clearOpen  = ref(false)
  const serverOpen = ref(false)
  
  const logKey    = ref(0)
  const usersKey  = ref(0)
  
  const applyTheme = () => document.documentElement.setAttribute("data-theme", themes[themeIdx.value])
  onMounted(applyTheme)
  
  watch(currView, v => {
    document.title = v === "log" ? "Log — RFID Admin" : "Users — RFID Admin"
  }, { immediate: true })
  
  const openMenu     = () => { panelState.value = "menu";     menuVsbl.value = true;  settVsbl.value = false }
  const openSettings = () => { panelState.value = "settings"; settVsbl.value = true;  menuVsbl.value = false }
  const closeAll     = () => { panelState.value = "none";     menuVsbl.value = false; settVsbl.value = false }
  
  const goView    = view => { currView.value = view; closeAll() }
  const openAdd    = () => { addOpen.value    = true; closeAll() }
  const openRemove = () => { removeOpen.value = true; closeAll() }
  const openEdit   = () => { editOpen.value   = true; closeAll() }
  const openClear  = () => { clearOpen.value  = true; closeAll() }
  const openServer = () => { serverOpen.value = true; closeAll() }
  
  const onUsersDone = () => { usersKey.value++; addOpen.value = removeOpen.value = editOpen.value = false }
  const onLogDone   = () => { logKey.value++;   clearOpen.value = false }
  
  const themeIcon = computed(() => {
    const t = themes[themeIdx.value]
    if (t === "system") return "IconRoutine"
    if (t === "light")  return "IconLightMode"
    return "IconDarkMode"
  })
  
  const toggleTheme = () => {
    themeIdx.value = (themeIdx.value + 1) % themes.length
    applyTheme()
  }
  
  const logout = async () => {
    const res  = await fetch(`${base.value}/api/auth/logout`, { credentials: "include" })
    const data = await res.json()
    if (data.success && !data.isLoggedIn) window.location.reload()
  }
  
  const openHelp = () => window.open("/help", "_blank")
</script>
<template>
  <Log   v-if="currView === 'log'"   :key="logKey"  />
  <Users v-else                      :key="usersKey"/>
  
  <button
    v-show="panelState === 'none'"
    :class="panelState === 'none' ? 'pop-in' : 'pop-out'"
    @click="openMenu"
    class="menuButton"
  ><IconMenu/></button>
  
  <vue-draggable-resizable
    v-show="menuVsbl"
    :x="posi.x" :y="posi.y" :z="1000" :w="'auto'" :h="'auto'"
    :resizable="false" :lock-aspect-ratio="true"
    @dragStop="(x, y) => { posi.x = x; posi.y = y }"
  >
    <div :class="panelState === 'menu' ? 'swap-in-left' : 'swap-out-right'">
      <button @mousedown.stop @click="openAdd"         title="Add User"   ><IconPersonAdd/></button>
      <button @mousedown.stop @click="openRemove"      title="Remove User"><IconPersonRemove/></button>
      <button @mousedown.stop @click="openEdit"        title="Edit User"  ><IconPersonEdit/></button>
      <button @mousedown.stop @click="goView('log')"   title="View Log"   ><IconHistory/></button>
      <button @mousedown.stop @click="goView('users')" title="View Users" ><IconPerson/></button>
      <button @mousedown.stop @click="openClear"       title="Clear Log"  ><IconDelete/></button>
      <button @mousedown.stop @click="openSettings"    title="Settings"   ><IconSettings/></button>
      <button @mousedown.stop @click="closeAll"        title="Close"      ><IconClose/></button>
    </div>
  </vue-draggable-resizable>
  
  <vue-draggable-resizable
    v-show="settVsbl"
    :x="posi.x" :y="posi.y" :z="1000" :w="'auto'" :h="'auto'"
    :resizable="false" :lock-aspect-ratio="true"
    @dragStop="(x, y) => { posi.x = x; posi.y = y }"
  >
    <div :class="panelState === 'settings' ? 'swap-in-right' : 'swap-out-left'">
      <button @mousedown.stop @click="toggleTheme"><component :is="themeIcon"/></button>
      <button @mousedown.stop @click="openServer"><IconDns/></button>
      <button @mousedown.stop @click="logout"><IconLogout/></button>
      <button @mousedown.stop @click="openHelp"><IconHelp/></button>
      <button @mousedown.stop @click="openMenu"><IconArrowBack/></button>
    </div>
  </vue-draggable-resizable>
  
  <AddUser    :visible="addOpen"    @close="addOpen    = false" @done="onUsersDone" />
  <RemoveUser :visible="removeOpen" @close="removeOpen = false" @done="onUsersDone" />
  <EditUser   :visible="editOpen"   @close="editOpen   = false" @done="onUsersDone" />
  <ClearLog   :visible="clearOpen"  @close="clearOpen  = false" @done="onLogDone"   />
  <EditServer :visible="serverOpen" @close="serverOpen = false" @done=""            />
</template>
<style scoped>
  .vdr {
    background: transparent; border: none;
    div {
      box-shadow: 0 0 5px 0.5px var(--foreground);
      grid-template-columns: repeat(3, 1fr);
      grid-template-rows: repeat(3, 1fr);
      backdrop-filter: blur(2.5px);
      background: transparent; border-radius: 15px;
      aspect-ratio: 1; display: grid; padding: 5px;
    }
  }
  .menuButton {
    margin: 0 20px 20px 0;
    position: fixed;
    bottom: 0;
    right: 0;
  }
  button {
    background:var(--background);
    border-radius: 15px;
    aspect-ratio: 1;
    padding: 10px;
    margin: 5px;
  }
  
  @keyframes popIn {
    0% {
      transform: scale(0.5);
      opacity: 0
    }
    70% {
      transform: scale(1.08);
      opacity: 1;
    }
    100% {
      transform: scale(1);
      opacity: 1;
    }
  }
  @keyframes popOut {
    0% {
      transform: scale(1);
      opacity: 1;
    }
    100% {
      transform: scale(0.5);
      opacity: 0;
    }
  }
  @keyframes swapInRight {
    0% {
      transform: scale(0.85) translateX(30px);
      opacity: 0;
    }
    70% {
      transform: scale(1.04) translateX(-4px);
      opacity: 1;
    }
    100% {
      transform: scale(1) translateX(0);
      opacity: 1;
    }
  }
  @keyframes swapOutLeft {
    0% {
      transform: scale(1) translateX(0);
      opacity: 1;
    }
    100% {
      transform: scale(0.85) translateX(-30px);
      opacity: 0;
    }
  }
  @keyframes swapInLeft {
    0% {
      transform: scale(0.85) translateX(-30px);
      opacity: 0;
    }
    70% {
      transform: scale(1.04) translateX(4px);
      opacity: 1;
    }
    100% {
      transform: scale(1) translateX(0);
      opacity: 1;
    }
  }
  @keyframes swapOutRight {
    0% {
      transform: scale(1) translateX(0);
      opacity: 1;
    }
    100% {
      transform: scale(0.85) translateX(30px);
      opacity: 0;
    }
  }
  
  .pop-in        { animation: popIn        200ms  cubic-bezier(0.34,1.56,0.64,1) forwards; }
  .pop-out       { animation: popOut       150ms ease-in forwards; }
  .swap-in-right { animation: swapInRight  220ms cubic-bezier(0.34,1.4,0.64,1) forwards; }
  .swap-out-left { animation: swapOutLeft  150ms ease-in forwards; }
  .swap-in-left  { animation: swapInLeft   220ms cubic-bezier(0.34,1.4,0.64,1) forwards; }
  .swap-out-right{ animation: swapOutRight 150ms ease-in forwards; }
</style>

<style>@import "vue-draggable-resizable/style.css"</style>