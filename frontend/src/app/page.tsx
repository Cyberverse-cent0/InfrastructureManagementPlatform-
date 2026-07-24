'use client';

import { Layout } from '@/components/layout/Layout';
import { useDashboardStats, useActivities, useTimeSeriesData } from '@/hooks/useDashboard';
import { formatRelativeTime, formatPercentage } from '@/lib/utils';
import { 
  Server, 
  Database, 
  HardDrive, 
  Network, 
  Activity as ActivityIcon,
  TrendingUp,
  TrendingDown,
  Clock,
  CheckCircle,
  AlertTriangle,
  XCircle
} from 'lucide-react';
import Badge from '@/components/ui/Badge';
import Button from '@/components/ui/Button';

export default function DashboardPage() {
  const { data: stats, isLoading: statsLoading } = useDashboardStats();
  const { data: activities, isLoading: activitiesLoading } = useActivities();
  const { data: cpuData } = useTimeSeriesData('cpu', 24);
  const { data: memoryData } = useTimeSeriesData('memory', 24);

  const statCards = [
    {
      title: 'Total Assets',
      value: stats?.totalAssets || 0,
      icon: Server,
      color: 'accent-blue',
      trend: '+12.5%',
      trendUp: true,
    },
    {
      title: 'Active',
      value: stats?.activeAssets || 0,
      icon: CheckCircle,
      color: 'status-success',
      trend: '+5.2%',
      trendUp: true,
    },
    {
      title: 'Maintenance',
      value: stats?.maintenanceAssets || 0,
      icon: AlertTriangle,
      color: 'status-warning',
      trend: '-2.1%',
      trendUp: false,
    },
    {
      title: 'Errors',
      value: stats?.errorAssets || 0,
      icon: XCircle,
      color: 'status-error',
      trend: '+1.3%',
      trendUp: false,
    },
  ];

  const assetTypeCards = [
    { name: 'Servers', count: stats?.totalServers || 0, icon: Server, color: 'accent-blue' },
    { name: 'Databases', count: stats?.totalDatabases || 0, icon: Database, color: 'accent-purple' },
    { name: 'Storage', count: stats?.totalStorage || 0, icon: HardDrive, color: 'accent-green' },
    { name: 'Network', count: stats?.totalNetwork || 0, icon: Network, color: 'accent-orange' },
  ];

  return (
    <Layout>
      <div className="space-y-6">
        {/* Header */}
        <div className="flex items-center justify-between">
          <div>
            <h1 className="text-3xl font-bold text-text-primary">Dashboard</h1>
            <p className="text-text-secondary mt-1">Overview of your infrastructure</p>
          </div>
          <div className="flex space-x-2">
            <Button variant="secondary">Export Report</Button>
            <Button>+ Add Asset</Button>
          </div>
        </div>

        {/* Stats Grid */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
          {statCards.map((card) => (
            <div
              key={card.title}
              className="bg-background-medium rounded-lg p-6 border border-border-color hover:border-accent-blue/50 transition-colors"
            >
              <div className="flex items-center justify-between">
                <div className="flex items-center space-x-3">
                  <div className={`w-10 h-10 rounded-lg bg-${card.color}/20 flex items-center justify-center`}>
                    <card.icon className={`w-5 h-5 text-${card.color}`} />
                  </div>
                  <div>
                    <p className="text-sm text-text-secondary">{card.title}</p>
                    <p className="text-2xl font-bold text-text-primary">{card.value}</p>
                  </div>
                </div>
                <div className={`flex items-center space-x-1 text-sm ${card.trendUp ? 'text-status-success' : 'text-status-error'}`}>
                  {card.trendUp ? (
                    <TrendingUp className="w-4 h-4" />
                  ) : (
                    <TrendingDown className="w-4 h-4" />
                  )}
                  <span>{card.trend}</span>
                </div>
              </div>
            </div>
          ))}
        </div>

        {/* Charts and Activity */}
        <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
          {/* Resource Usage Chart */}
          <div className="lg:col-span-2 bg-background-medium rounded-lg p-6 border border-border-color">
            <h2 className="text-lg font-semibold text-text-primary mb-4">Resource Usage</h2>
            <div className="h-64 flex items-center justify-center bg-background-light/50 rounded-lg">
              <div className="text-center">
                <ActivityIcon className="w-12 h-12 text-text-secondary mx-auto mb-2" />
                <p className="text-text-secondary">Chart visualization would go here</p>
                <p className="text-sm text-text-muted mt-1">CPU: {formatPercentage(stats?.avgCpuUsage || 0)} | Memory: {formatPercentage(stats?.avgMemoryUsage || 0)}</p>
              </div>
            </div>
          </div>

          {/* Asset Types */}
          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <h2 className="text-lg font-semibold text-text-primary mb-4">Asset Distribution</h2>
            <div className="space-y-3">
              {assetTypeCards.map((card) => (
                <div key={card.name} className="flex items-center justify-between">
                  <div className="flex items-center space-x-3">
                    <card.icon className={`w-5 h-5 text-${card.color}`} />
                    <span className="text-text-primary">{card.name}</span>
                  </div>
                  <Badge variant="default">{card.count}</Badge>
                </div>
              ))}
            </div>
          </div>
        </div>

        {/* Recent Activity */}
        <div className="bg-background-medium rounded-lg p-6 border border-border-color">
          <div className="flex items-center justify-between mb-4">
            <h2 className="text-lg font-semibold text-text-primary">Recent Activity</h2>
            <Button variant="ghost" size="sm">View All</Button>
          </div>
          
          {activitiesLoading ? (
            <div className="text-center py-8">
              <ActivityIcon className="w-8 h-8 text-text-secondary mx-auto animate-pulse" />
              <p className="text-text-secondary mt-2">Loading activities...</p>
            </div>
          ) : activities && activities.length > 0 ? (
            <div className="space-y-4">
              {activities.map((activity) => (
                <div key={activity.id} className="flex items-start space-x-3 p-3 bg-background-light/50 rounded-lg">
                  <div className="w-8 h-8 rounded-full bg-accent-purple/20 flex items-center justify-center text-xs font-medium text-accent-purple">
                    {activity.userName.split(' ').map(n => n[0]).join('')}
                  </div>
                  <div className="flex-1 min-w-0">
                    <p className="text-sm text-text-primary">{activity.message}</p>
                    <div className="flex items-center space-x-2 mt-1">
                      <Clock className="w-3 h-3 text-text-secondary" />
                      <span className="text-xs text-text-secondary">{formatRelativeTime(activity.timestamp)}</span>
                    </div>
                  </div>
                  <Badge variant={activity.type === 'error' ? 'error' : activity.type === 'create' ? 'success' : 'default'}>
                    {activity.type}
                  </Badge>
                </div>
              ))}
            </div>
          ) : (
            <div className="text-center py-8">
              <ActivityIcon className="w-8 h-8 text-text-secondary mx-auto" />
              <p className="text-text-secondary mt-2">No recent activity</p>
            </div>
          )}
        </div>

        {/* Performance Metrics */}
        <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <div className="flex items-center justify-between mb-2">
              <span className="text-text-secondary text-sm">Average CPU Usage</span>
              <TrendingUp className="w-4 h-4 text-status-success" />
            </div>
            <p className="text-2xl font-bold text-text-primary">{formatPercentage(stats?.avgCpuUsage || 0)}</p>
            <div className="mt-2 h-2 bg-background-light rounded-full overflow-hidden">
              <div 
                className="h-full bg-accent-blue transition-all duration-500"
                style={{ width: `${stats?.avgCpuUsage || 0}%` }}
              />
            </div>
          </div>

          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <div className="flex items-center justify-between mb-2">
              <span className="text-text-secondary text-sm">Average Memory Usage</span>
              <TrendingUp className="w-4 h-4 text-status-warning" />
            </div>
            <p className="text-2xl font-bold text-text-primary">{formatPercentage(stats?.avgMemoryUsage || 0)}</p>
            <div className="mt-2 h-2 bg-background-light rounded-full overflow-hidden">
              <div 
                className="h-full bg-accent-purple transition-all duration-500"
                style={{ width: `${stats?.avgMemoryUsage || 0}%` }}
              />
            </div>
          </div>

          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <div className="flex items-center justify-between mb-2">
              <span className="text-text-secondary text-sm">System Uptime</span>
              <CheckCircle className="w-4 h-4 text-status-success" />
            </div>
            <p className="text-2xl font-bold text-text-primary">{stats?.uptime || 0}%</p>
            <div className="mt-2 h-2 bg-background-light rounded-full overflow-hidden">
              <div 
                className="h-full bg-status-success transition-all duration-500"
                style={{ width: `${stats?.uptime || 0}%` }}
              />
            </div>
          </div>
        </div>
      </div>
    </Layout>
  );
}
