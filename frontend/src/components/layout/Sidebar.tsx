'use client';

import { useState } from 'react';
import Link from 'next/link';
import { usePathname } from 'next/navigation';
import { cn } from '@/lib/utils';
import { 
  LayoutDashboard, 
  Server, 
  Database, 
  HardDrive, 
  Network, 
  Activity,
  Settings,
  Menu,
  X
} from 'lucide-react';
import { useGlobalStore } from '@/store/globalStore';

const navigation = [
  { name: 'Dashboard', href: '/', icon: LayoutDashboard },
  { name: 'Inventory', href: '/inventory', icon: Server },
  { name: 'Analytics', href: '/analytics', icon: Activity },
  { name: 'Settings', href: '/settings', icon: Settings },
];

const assetTypes = [
  { name: 'Servers', href: '/inventory?type=server', icon: Server },
  { name: 'Databases', href: '/inventory?type=database', icon: Database },
  { name: 'Storage', href: '/inventory?type=storage', icon: HardDrive },
  { name: 'Network', href: '/inventory?type=network', icon: Network },
];

export function Sidebar() {
  const pathname = usePathname();
  const sidebarOpen = useGlobalStore((state) => state.sidebarOpen);
  const toggleSidebar = useGlobalStore((state) => state.toggleSidebar);
  const [assetSectionOpen, setAssetSectionOpen] = useState(true);

  return (
    <>
      {/* Mobile backdrop */}
      {sidebarOpen && (
        <div
          className="fixed inset-0 bg-black/50 z-40 lg:hidden"
          onClick={toggleSidebar}
        />
      )}
      
      {/* Sidebar */}
      <aside
        className={cn(
          'fixed left-0 top-0 z-50 h-full w-64 bg-background-medium border-r border-border-color transition-transform duration-300 lg:translate-x-0',
          sidebarOpen ? 'translate-x-0' : '-translate-x-full'
        )}
      >
        <div className="flex h-full flex-col">
          {/* Logo */}
          <div className="flex h-16 items-center justify-between px-6 border-b border-border-color">
            <div className="flex items-center space-x-2">
              <div className="w-8 h-8 bg-gradient-to-br from-accent-blue to-accent-purple rounded-lg flex items-center justify-center">
                <Server className="w-5 h-5 text-white" />
              </div>
              <span className="text-lg font-bold text-text-primary">IMP</span>
            </div>
            <button
              onClick={toggleSidebar}
              className="lg:hidden text-text-secondary hover:text-text-primary"
            >
              <X className="w-5 h-5" />
            </button>
          </div>

          {/* Navigation */}
          <nav className="flex-1 overflow-y-auto px-3 py-4 space-y-1">
            {/* Main Navigation */}
            <div className="space-y-1">
              {navigation.map((item) => {
                const isActive = pathname === item.href;
                return (
                  <Link
                    key={item.name}
                    href={item.href}
                    className={cn(
                      'flex items-center space-x-3 px-3 py-2 rounded-lg text-sm font-medium transition-colors',
                      isActive
                        ? 'bg-accent-blue/10 text-accent-blue'
                        : 'text-text-secondary hover:text-text-primary hover:bg-background-light'
                    )}
                  >
                    <item.icon className="w-5 h-5" />
                    <span>{item.name}</span>
                  </Link>
                );
              })}
            </div>

            {/* Asset Types Section */}
            <div className="mt-6">
              <button
                onClick={() => setAssetSectionOpen(!assetSectionOpen)}
                className="flex items-center justify-between w-full px-3 py-2 text-sm font-medium text-text-secondary hover:text-text-primary"
              >
                <span>Asset Types</span>
                <svg
                  className={cn(
                    'w-4 h-4 transition-transform',
                    assetSectionOpen ? 'rotate-180' : ''
                  )}
                  fill="none"
                  stroke="currentColor"
                  viewBox="0 0 24 24"
                >
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M19 9l-7 7-7-7" />
                </svg>
              </button>
              
              {assetSectionOpen && (
                <div className="mt-1 space-y-1">
                  {assetTypes.map((item) => {
                    const isActive = pathname === item.href;
                    return (
                      <Link
                        key={item.name}
                        href={item.href}
                        className={cn(
                          'flex items-center space-x-3 px-3 py-2 rounded-lg text-sm font-medium transition-colors ml-4',
                          isActive
                            ? 'bg-accent-blue/10 text-accent-blue'
                            : 'text-text-secondary hover:text-text-primary hover:bg-background-light'
                        )}
                      >
                        <item.icon className="w-4 h-4" />
                        <span>{item.name}</span>
                      </Link>
                    );
                  })}
                </div>
              )}
            </div>
          </nav>

          {/* User Section */}
          <div className="border-t border-border-color p-4">
            <div className="flex items-center space-x-3">
              <div className="w-8 h-8 bg-accent-purple rounded-full flex items-center justify-center text-white font-medium">
                JD
              </div>
              <div className="flex-1 min-w-0">
                <p className="text-sm font-medium text-text-primary truncate">John Doe</p>
                <p className="text-xs text-text-secondary truncate">john@example.com</p>
              </div>
            </div>
          </div>
        </div>
      </aside>
    </>
  );
}
