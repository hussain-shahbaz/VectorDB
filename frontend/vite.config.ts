import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vite.dev/config/
export default defineConfig({
  plugins: [react()],
  server: {
    proxy: {
      // Lets the frontend call relative /api/... paths in dev without
      // needing CORS at all (the server also sets CORS headers itself,
      // for anyone hitting it directly without this proxy).
      '/api': {
        target: 'http://localhost:8080',
        changeOrigin: true,
      },
    },
  },
})
