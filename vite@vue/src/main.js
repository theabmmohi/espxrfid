import { createApp, defineAsyncComponent } from "vue"
import "./style.css"
import App from "./App.vue"

const app   = createApp(App)
const icons = import.meta.glob("./assets/*.svg", { query: "?component"})
const comps = import.meta.glob("./components/*.vue")
for (const path in icons) {
  const icon = path
    .split("/")
    .pop()
    .replace(".svg", "")
    .split("_")
    .map(word => word.charAt(0).toUpperCase() + word.slice(1))
    .join("")
  const component = `Icon${icon}`
  app.component(component, defineAsyncComponent(icons[path]))
}

app.mount("#app")