<script setup>
  import { ref, onMounted, onUnmounted } from "vue"
  import { base } from "../base.js"
  
  const log    = ref([])
  const newSls = ref([])
  let   es     = null
  
  const getLog = async () => {
    const response = await fetch(`${base.value}/api/view/log`, { credentials: "include" })
    const res = await response.json()
    if (res.success) log.value = res.data
  }
  
  const connectSSE = () => {
    es = new EventSource(`${base.value}/api/events`)
    es.onmessage = (event) => {
      const row = JSON.parse(event.data)
      if (log.value.some(r => r.sl === row.sl)) return
      log.value.unshift(row)
      newSls.value.push(row.sl)
      setTimeout(() => { newSls.value = newSls.value.filter(s => s !== row.sl) }, 2000)
    }
  }
  
  const copyUID = (uid) => navigator.clipboard?.writeText(uid)
  
  onMounted(async () => { await getLog(); connectSSE() })
  onUnmounted(() => es?.close())
</script>

<template>
  <div class="table-wrap">
    <table class="log-table">
      <thead>
        <tr>
          <th>SL</th><th>Name</th><th>Type</th><th>Time</th>
          <th title="Click to copy">UID</th><th>Access</th>
        </tr>
      </thead>
      <TransitionGroup tag="tbody" name="row">
        <tr
          v-for="row in log" :key="row.sl"
          :class="[row.access ? 'row-yes' : 'row-no', newSls.includes(row.sl) ? 'row-pulse' : '']"
        >
          <td class="center">{{ row.sl }}</td>
          <td>{{ row.name }}</td>
          <td>{{ row.type }}</td>
          <td class="center time-cell">{{ row.time }}</td>
          <td class="center uid-cell" @click="copyUID(row.uid)" title="Click to copy">{{ row.uid }}</td>
          <td class="center">{{ row.access ? "Granted" : "Denied" }}</td>
        </tr>
        <tr v-if="!log.length" key="empty">
          <td colspan="6" class="empty">No log entries.</td>
        </tr>
      </TransitionGroup>
    </table>
  </div>
</template>

<style scoped>
  .table-wrap {
    overflow: auto;
    padding: 16px;
    height: 100dvh;
  }
  .log-table {
    border-collapse: collapse;
    min-width: 100%;
  }
  .log-table, th, td { border: 1px solid var(--foreground); }
  th {
    background: var(--background);
    top: 0; z-index: 1;
    padding: 4px 8px;
    position: sticky;
  }
  th:nth-child(2), th:nth-child(3), th:nth-child(4) { min-width: 12ch; }
  td { padding: 4px 6px; }
  .center { text-align: center; }
  .time-cell {
    white-space: pre-line;
    font-size: 0.85rem;
  }
  .uid-cell {
    font-family: monospace;
    cursor: pointer;
  }
  .uid-cell:hover { opacity: 0.7; }
  .row-yes { background: var(--g); }
  .row-no  { background: var(--r); }
  
  @keyframes pulse {
    0% {filter: brightness(1.65) saturate(1.5); }
    100% { filter: brightness(1) saturate(1); }
  }
  .row-pulse { animation: pulse 2s ease-out forwards; }
  .row-enter-active {
    transition: opacity 0.3s ease,
                transform 0.38s cubic-bezier(0.34, 1.5, 0.64, 1),
                max-height 0.3s ease;
  }
  .row-enter-from { opacity: 0; transform: translateY(-24px); }
  .row-enter-to   { opacity: 1; transform: translateY(0); }
  .row-move       { transition: transform 0.32s ease; }
  .empty {
    color: var(--foreground);
    text-align: center;
    padding: 20px;
    opacity: 0.5;
  }
</style>