import { create } from 'zustand';
import type { Asset, FilterOptions, SortOptions, Notification, UserPreferences } from '@/types';

interface AppState {
  // UI State
  sidebarOpen: boolean;
  theme: 'dark' | 'light';
  notifications: Notification[];
  
  // Inventory State
  selectedAssets: Set<string>;
  filters: FilterOptions;
  sort: SortOptions;
  
  // User Preferences
  preferences: UserPreferences;
  
  // Actions
  toggleSidebar: () => void;
  setTheme: (theme: 'dark' | 'light') => void;
  addNotification: (notification: Omit<Notification, 'id' | 'timestamp' | 'read'>) => void;
  removeNotification: (id: string) => void;
  markNotificationRead: (id: string) => void;
  
  // Asset Actions
  setSelectedAssets: (assets: Set<string>) => void;
  toggleAssetSelection: (id: string) => void;
  clearSelectedAssets: () => void;
  setFilters: (filters: FilterOptions) => void;
  setSort: (sort: SortOptions) => void;
  
  // Preferences Actions
  setPreferences: (preferences: Partial<UserPreferences>) => void;
}

export const useGlobalStore = create<AppState>((set) => ({
  // Initial State
  sidebarOpen: true,
  theme: 'dark',
  notifications: [],
  selectedAssets: new Set(),
  filters: {},
  sort: { field: 'name', direction: 'asc' },
  preferences: {
    theme: 'dark',
    language: 'en',
    dashboardLayout: 'grid',
    defaultView: 'dashboard',
    notifications: true,
    autoRefresh: true,
    refreshInterval: 30000,
  },
  
  // UI Actions
  toggleSidebar: () => set((state) => ({ sidebarOpen: !state.sidebarOpen })),
  
  setTheme: (theme) => set({ theme }),
  
  addNotification: (notification) => set((state) => ({
    notifications: [
      {
        ...notification,
        id: Date.now().toString(),
        timestamp: new Date(),
        read: false,
      },
      ...state.notifications,
    ].slice(0, 50), // Keep only last 50 notifications
  })),
  
  removeNotification: (id) => set((state) => ({
    notifications: state.notifications.filter((n) => n.id !== id),
  })),
  
  markNotificationRead: (id) => set((state) => ({
    notifications: state.notifications.map((n) =>
      n.id === id ? { ...n, read: true } : n
    ),
  })),
  
  // Asset Actions
  setSelectedAssets: (assets) => set({ selectedAssets: assets }),
  
  toggleAssetSelection: (id) => set((state) => {
    const newSelected = new Set(state.selectedAssets);
    if (newSelected.has(id)) {
      newSelected.delete(id);
    } else {
      newSelected.add(id);
    }
    return { selectedAssets: newSelected };
  }),
  
  clearSelectedAssets: () => set({ selectedAssets: new Set() }),
  
  setFilters: (filters) => set({ filters }),
  
  setSort: (sort) => set({ sort }),
  
  // Preferences Actions
  setPreferences: (preferences) => set((state) => ({
    preferences: { ...state.preferences, ...preferences },
  })),
}));
