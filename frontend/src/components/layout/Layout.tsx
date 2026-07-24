'use client';

import { ReactNode } from 'react';
import { Sidebar } from './Sidebar';
import { Header } from './Header';
import { AuthGuard } from './AuthGuard';
import { useGlobalStore } from '@/store/globalStore';
import { cn } from '@/lib/utils';

interface LayoutProps {
  children: ReactNode;
}

export function Layout({ children }: LayoutProps) {
  const sidebarOpen = useGlobalStore((state) => state.sidebarOpen);

  return (
    <AuthGuard>
      <div className="min-h-screen bg-background-dark">
        <Sidebar />
        <div
          className={cn(
            'transition-all duration-300',
            sidebarOpen ? 'lg:ml-64' : 'lg:ml-0'
          )}
        >
          <Header />
          <main className="p-4 lg:p-6">
            {children}
          </main>
        </div>
      </div>
    </AuthGuard>
  );
}
