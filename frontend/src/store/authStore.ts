import { create } from 'zustand';
import type { User, AuthState } from '@/types/auth';

interface AuthStore extends AuthState {
  login: (email: string, password: string) => Promise<void>;
  logout: () => void;
  setUser: (user: User | null) => void;
}

export const useAuthStore = create<AuthStore>((set) => ({
  user: null,
  isAuthenticated: false,
  isLoading: false,

  login: async (email: string, password: string) => {
    set({ isLoading: true });
    
    // Simulate API call
    await new Promise(resolve => setTimeout(resolve, 1000));
    
    // Mock authentication - replace with real API call
    if (email === 'admin@example.com' && password === 'admin123') {
      const user: User = {
        id: '1',
        email: 'admin@example.com',
        name: 'John Doe',
        role: 'admin',
      };
      set({ user, isAuthenticated: true, isLoading: false });
      localStorage.setItem('user', JSON.stringify(user));
    } else if (email === 'user@example.com' && password === 'user123') {
      const user: User = {
        id: '2',
        email: 'user@example.com',
        name: 'Jane Smith',
        role: 'user',
      };
      set({ user, isAuthenticated: true, isLoading: false });
      localStorage.setItem('user', JSON.stringify(user));
    } else {
      set({ isLoading: false });
      throw new Error('Invalid credentials');
    }
  },

  logout: () => {
    set({ user: null, isAuthenticated: false });
    localStorage.removeItem('user');
  },

  setUser: (user) => {
    set({ user, isAuthenticated: !!user });
    if (user) {
      localStorage.setItem('user', JSON.stringify(user));
    } else {
      localStorage.removeItem('user');
    }
  },
}));

// Initialize auth state from localStorage
if (typeof window !== 'undefined') {
  const savedUser = localStorage.getItem('user');
  if (savedUser) {
    try {
      const user = JSON.parse(savedUser);
      useAuthStore.getState().setUser(user);
    } catch (e) {
      localStorage.removeItem('user');
    }
  }
}
