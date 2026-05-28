import { ref } from "vue"

const savedBase = localStorage.getItem("serverBase")
export const base = ref(savedBase || `${window.location.protocol}//${window.location.host}`)
