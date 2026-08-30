import { defineStore } from 'pinia'
import { ref, computed } from 'vue'
export const useAuthStore = defineStore('auth', () => {
  // --- State ---
  const user = ref(JSON.parse(localStorage.getItem('user')) || null)
  const token = ref(localStorage.getItem('token') || null)
  const loading = ref(false)
  const error = ref(null)

  // --- Getters ---
  const isAuthenticated = computed(() => !!token.value)

  // --- Actions ---
  async function login(credentials) {
    loading.value = true
    error.value = null
    try {
      // Replace with your actual backend API endpoint
      
      // Assume API returns { user: { id: 1, name: 'John' }, token: 'abc...' }
      user.value = response.data.user
      token.value = response.data.token

      // Persist data so it survives page reloads
      localStorage.setItem('user', JSON.stringify(user.value))
      localStorage.setItem('token', token.value)
      
    } catch (err) {
      error.value = err.response?.data?.message || 'Login failed'
      throw err
    } finally {
      loading.value = false
    }
  }

  function logout() {
    user.value = null
    token.value = null
    localStorage.removeItem('user')
    localStorage.removeItem('token')
  }

  return {
    user,
    token,
    loading,
    error,
    isAuthenticated,
    login,
    logout
  }
})