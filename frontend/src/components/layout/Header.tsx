'use client';

import { useState } from 'react';
import { useRouter } from 'next/navigation';
import { Search, Bell, Settings, User, Moon, Sun, LogOut } from 'lucide-react';
import { cn } from '@/lib/utils';
import { useGlobalStore } from '@/store/globalStore';
import { useAuthStore } from '@/store/authStore';
import Button from '@/components/ui/Button';
import Badge from '@/components/ui/Badge';

export function Header() {
  const router = useRouter();
  const [searchQuery, setSearchQuery] = useState('');
  const theme = useGlobalStore((state) => state.theme);
  const setTheme = useGlobalStore((state) => state.setTheme);
  const toggleSidebar = useGlobalStore((state) => state.toggleSidebar);
  const notifications = useGlobalStore((state) => state.notifications);
  const unreadCount = notifications.filter(n => !n.read).length;
  const { user, logout } = useAuthStore();

  const toggleTheme = () => {
    setTheme(theme === 'dark' ? 'light' : 'dark');
  };

  const handleLogout = () => {
    logout();
    router.push('/login');
  };

  const handleSearch = (e: React.FormEvent) => {
    e.preventDefault();
    if (searchQuery.trim()) {
      router.push(`/inventory?search=${encodeURIComponent(searchQuery)}`);
    }
  };

  const getUserInitials = () => {
    if (user?.name) {
      return user.name.split(' ').map(n => n[0]).join('').toUpperCase();
    }
    return 'U';
  };

  return (
    <header className="h-16 bg-background-medium border-b border-border-color flex items-center justify-between px-4 lg:px-6">
      {/* Left side */}
      <div className="flex items-center space-x-4">
        <button
          onClick={toggleSidebar}
          className="lg:hidden text-text-secondary hover:text-text-primary"
        >
          <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
            <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 6h16M4 12h16M4 18h16" />
          </svg>
        </button>
        
        {/* Search */}
        <form onSubmit={handleSearch} className="relative hidden md:block">
          <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 w-4 h-4 text-text-secondary" />
          <input
            type="text"
            placeholder="Search assets..."
            value={searchQuery}
            onChange={(e) => setSearchQuery(e.target.value)}
            className="w-48 lg:w-64 pl-10 pr-4 py-2 bg-background-light border border-border-color rounded-lg text-sm text-text-primary placeholder-text-secondary focus:outline-none focus:ring-2 focus:ring-accent-blue"
          />
        </form>
      </div>

      {/* Right side */}
      <div className="flex items-center space-x-2 lg:space-x-4">
        {/* Theme Toggle */}
        <Button
          variant="ghost"
          size="sm"
          onClick={toggleTheme}
          className="hidden md:flex"
        >
          {theme === 'dark' ? (
            <Sun className="w-4 h-4" />
          ) : (
            <Moon className="w-4 h-4" />
          )}
        </Button>

        {/* Notifications */}
        <div className="relative">
          <Button variant="ghost" size="sm" className="relative">
            <Bell className="w-4 h-4" />
            {unreadCount > 0 && (
              <Badge
                variant="error"
                className="absolute -top-1 -right-1 w-4 h-4 flex items-center justify-center p-0 text-xs"
              >
                {unreadCount}
              </Badge>
            )}
          </Button>
        </div>

        {/* Settings */}
        <Button variant="ghost" size="sm" className="hidden md:flex" onClick={() => router.push('/settings')}>
          <Settings className="w-4 h-4" />
        </Button>

        {/* User */}
        <div className="flex items-center space-x-2 pl-2 border-l border-border-color">
          <div className="w-8 h-8 bg-accent-purple rounded-full flex items-center justify-center text-white font-medium text-sm">
            {getUserInitials()}
          </div>
          <div className="hidden lg:block">
            <p className="text-sm font-medium text-text-primary">{user?.name || 'User'}</p>
            <p className="text-xs text-text-secondary capitalize">{user?.role || 'Guest'}</p>
          </div>
          <Button variant="ghost" size="sm" onClick={handleLogout} title="Logout">
            <LogOut className="w-4 h-4" />
          </Button>
        </div>
      </div>
    </header>
  );
}
