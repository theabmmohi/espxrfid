<script setup>
  defineProps(["visible"])
  const emit = defineEmits(["close"])
</script>
<template>
  <Teleport to="body">
    <div v-if="visible" class="overlay" @click.self="emit('close')">
      <div class="box" @click.stop><slot /></div>
    </div>
  </Teleport>
</template>
<style scoped>
  .overlay {
    -webkit-backdrop-filter: blur(5px);
    background: rgba(0,0,0,0.35);
    animation: fadeIn 0.15s ease;
    backdrop-filter: blur(5px);
    justify-content: center;
    align-items: center;
    position: fixed;
    display: flex;
    z-index: 100;
    inset: 0;
  }
  .box {
    animation: slideUp 0.18s cubic-bezier(0.34, 1.4, 0.64, 1);
    box-shadow: 0 0 10px var(--foreground);
    border: 1px solid var(--foreground);
    background: var(--background);
    width: min(85vw, 360px);
    flex-direction: column;
    border-radius: 20px;
    padding: 25px 20px;
    display: flex;
    gap: 15px;
  }
  @keyframes fadeIn {
    from { opacity: 0; } to { opacity: 1; }
  }
  @keyframes slideUp {
    from {
      transform: translateY(20px) scale(0.95);
      opacity: 0;
    }
    to {
      transform: none;
      opacity: 1;
    }
  }
</style>