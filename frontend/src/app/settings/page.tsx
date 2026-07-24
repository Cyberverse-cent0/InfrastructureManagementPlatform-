'use client';

import { Layout } from '@/components/layout/Layout';
import { useAuthStore } from '@/store/authStore';
import { useGlobalStore } from '@/store/globalStore';
import { 
  User, 
  Bell, 
  Shield, 
  Palette, 
  Globe,
  Moon,
  Sun,
  LogOut,
  ChevronRight
} from 'lucide-react';
import Button from '@/components/ui/Button';
import Badge from '@/components/ui/Badge';
import { useRouter } from 'next/navigation';

export default function SettingsPage() {
  const router = useRouter();
  const { user, logout } = useAuthStore();
  const { theme, setTheme, toggleSidebar } = useGlobalStore();

  const handleLogout = () => {
    logout();
    router.push('/login');
  };

  const settingsSections = [
    {
      title: 'Profile',
      icon: User,
      items: [
        { label: 'Personal Information', value: user?.name || 'Not set' },
        { label: 'Email', value: user?.email || 'Not set' },
        { label: 'Role', value: user?.role || 'Not set' },
      ]
    },
    {
      title: 'Appearance',
      icon: Palette,
      items: [
        { 
          label: 'Theme', 
          value: theme === 'dark' ? 'Dark' : 'Light',
          action: () => setTheme(theme === 'dark' ? 'light' : 'dark'),
          actionIcon: theme === 'dark' ? Sun : Moon
        },
        { label: 'Sidebar', value: 'Collapsible' },
      ]
    },
    {
      title: 'Notifications',
      icon: Bell,
      items: [
        { label: 'Email Notifications', value: 'Enabled' },
        { label: 'Push Notifications', value: 'Enabled' },
        { label: 'Alert Frequency', value: 'Real-time' },
      ]
    },
    {
      title: 'Security',
      icon: Shield,
      items: [
        { label: 'Two-Factor Auth', value: 'Disabled' },
        { label: 'Last Login', value: '2024-07-24' },
        { label: 'Password Changed', value: 'Never' },
      ]
    },
    {
      title: 'Language & Region',
      icon: Globe,
      items: [
        { label: 'Language', value: 'English' },
        { label: 'Timezone', value: 'UTC' },
        { label: 'Date Format', value: 'MM/DD/YYYY' },
      ]
    }
  ];

  return (
    <Layout>
      <div className="space-y-6">
        {/* Header */}
        <div className="flex items-center justify-between">
          <div>
            <h1 className="text-3xl font-bold text-text-primary">Settings</h1>
            <p className="text-text-secondary mt-1">Manage your account and preferences</p>
          </div>
          <Button variant="danger" onClick={handleLogout}>
            <LogOut className="w-4 h-4 mr-2" />
            Logout
          </Button>
        </div>

        {/* Settings Sections */}
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          {settingsSections.map((section) => (
            <div key={section.title} className="bg-background-medium rounded-lg p-6 border border-border-color">
              <div className="flex items-center space-x-3 mb-4">
                <section.icon className="w-5 h-5 text-accent-blue" />
                <h2 className="text-lg font-semibold text-text-primary">{section.title}</h2>
              </div>
              
              <div className="space-y-3">
                {section.items.map((item, index) => (
                  <div 
                    key={index}
                    className="flex items-center justify-between py-2 border-b border-border-color last:border-0"
                  >
                    <div>
                      <p className="text-sm text-text-primary">{item.label}</p>
                    </div>
                    <div className="flex items-center space-x-2">
                      <span className="text-sm text-text-secondary">{item.value}</span>
                      {item.action && (
                        <Button 
                          variant="ghost" 
                          size="sm"
                          onClick={item.action}
                        >
                          {item.actionIcon && <item.actionIcon className="w-4 h-4" />}
                        </Button>
                      )}
                      <ChevronRight className="w-4 h-4 text-text-secondary" />
                    </div>
                  </div>
                ))}
              </div>
            </div>
          ))}
        </div>

        {/* Danger Zone */}
        <div className="bg-background-medium rounded-lg p-6 border border-status-error/30">
          <h2 className="text-lg font-semibold text-status-error mb-4">Danger Zone</h2>
          <div className="flex items-center justify-between">
            <div>
              <p className="text-text-primary">Delete Account</p>
              <p className="text-sm text-text-secondary">Permanently delete your account and all data</p>
            </div>
            <Button variant="danger">Delete Account</Button>
          </div>
        </div>
      </div>
    </Layout>
  );
}
