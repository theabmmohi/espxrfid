import { defineConfig } from "vite"
import vue from "@vitejs/plugin-vue"
import svgLoader from "vite-svg-loader"
import { resolve, dirname } from "node:path"
import { fileURLToPath } from "node:url"
import fs from "node:fs"
const __dirname = dirname(fileURLToPath(import.meta.url))
export default defineConfig({
  plugins: [
    vue(),
    svgLoader(),
    {
      name: 'move-files',
      closeBundle() {
        const oldIndex  = resolve(__dirname, "dist/index.html")
        const newIndex  = resolve(__dirname, "../templates/index.html")
        const oldAssets = resolve(__dirname, "dist/static/assets")
        const newAssets = resolve(__dirname, "../static/assets")
        
        const templatesDir = resolve(__dirname, "../templates")
        const staticDir = resolve(__dirname, "../static/assets")
        
        if (!fs.existsSync(templatesDir)) fs.mkdirSync(templatesDir, { recursive: true })
        if (fs.existsSync(oldIndex)) fs.renameSync(oldIndex, newIndex)
        if (fs.existsSync(staticDir)) fs.rmSync(staticDir, { recursive: true, force: true })
        if (!fs.existsSync(resolve(__dirname, "../static"))) fs.mkdirSync(resolve(__dirname, "../static"))
        if (fs.existsSync(oldAssets)) fs.renameSync(oldAssets, newAssets)
        
        if (fs.existsSync(resolve(__dirname, "dist"))) fs.rmSync(resolve(__dirname, "dist"), { recursive: true, force: true })
      }
    }
  ],
  build: {
    outDir: "dist",
    emptyOutDir: true, 
    rollupOptions: {
      output: {
        entryFileNames: "static/assets/[name]_[hash].js",
        chunkFileNames: "static/assets/[name]_[hash].js",
        assetFileNames: "static/assets/[name]_[hash].[ext]"
      }
    }
  },
  server: {
    allowedHosts: ["app.imabm.eu.cc"],
    port: 5000,
    proxy: {
      "/help": {
        target: "http://127.0.0.1:8000",
        changeOrigin: true,
        secure: false
      },
      "/api": {
        target: "http://127.0.0.1:8000",
        changeOrigin: true,
        secure: false
      }
    }
  }
})