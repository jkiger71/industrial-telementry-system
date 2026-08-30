import { fileURLToPath, URL } from 'node:url'
import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import vueDevTools from 'vite-plugin-vue-devtools'

// https://vite.dev/config/
export default defineConfig({
  plugins: [
    vue(),
    vueDevTools(),
  ],
  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url)),
    },
  },
  server: {
    host: true, // Crucial for Docker: exposes the container so you can access it from your browser
    port: 5173,
    proxy: {
      // Whenever Vue hits "/api", Vite proxies it to your Django Docker service
      '/api': {
        target: 'http://backend:8000', // Matches your Django service name in docker-compose.yml
        changeOrigin: true,
      }
    }
  }
})
