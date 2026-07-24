'use client';

import { useState } from 'react';
import { Layout } from '@/components/layout/Layout';
import { useDashboardStats, useTimeSeriesData } from '@/hooks/useDashboard';
import { formatPercentage } from '@/lib/utils';
import {
  BarChart3,
  LineChart,
  PieChart,
  TrendingUp,
  Cpu,
  HardDrive,
  Network,
  Activity
} from 'lucide-react';
import Button from '@/components/ui/Button';

export default function AnalyticsPage() {
  const { data: stats } = useDashboardStats();
  const { data: cpuData } = useTimeSeriesData('cpu', 24);
  const { data: memoryData } = useTimeSeriesData('memory', 24);
  const { data: networkData } = useTimeSeriesData('network', 24);

  const timeRanges = [
    { label: '24h', value: 24 },
    { label: '7d', value: 168 },
    { label: '30d', value: 720 },
  ];

  const [selectedRange, setSelectedRange] = useState(24);

  return (
    <Layout>
      <div className="space-y-6">
        {/* Header */}
        <div className="flex items-center justify-between">
          <div>
            <h1 className="text-3xl font-bold text-text-primary">Analytics</h1>
            <p className="text-text-secondary mt-1">Performance metrics and insights</p>
          </div>
          <div className="flex items-center space-x-2">
            {timeRanges.map((range) => (
              <Button
                key={range.value}
                variant={selectedRange === range.value ? 'primary' : 'secondary'}
                size="sm"
                onClick={() => setSelectedRange(range.value)}
              >
                {range.label}
              </Button>
            ))}
          </div>
        </div>

        {/* KPI Cards */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <div className="flex items-center justify-between mb-4">
              <Cpu className="w-5 h-5 text-accent-blue" />
              <span className="text-xs text-text-secondary">CPU Usage</span>
            </div>
            <p className="text-2xl font-bold text-text-primary">{formatPercentage(stats?.avgCpuUsage || 0)}</p>
            <div className="flex items-center mt-2 text-sm text-status-success">
              <TrendingUp className="w-4 h-4 mr-1" />
              <span>+5.2%</span>
            </div>
          </div>

          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <div className="flex items-center justify-between mb-4">
              <Activity className="w-5 h-5 text-accent-purple" />
              <span className="text-xs text-text-secondary">Memory Usage</span>
            </div>
            <p className="text-2xl font-bold text-text-primary">{formatPercentage(stats?.avgMemoryUsage || 0)}</p>
            <div className="flex items-center mt-2 text-sm text-status-warning">
              <TrendingUp className="w-4 h-4 mr-1" />
              <span>+2.1%</span>
            </div>
          </div>

          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <div className="flex items-center justify-between mb-4">
              <HardDrive className="w-5 h-5 text-accent-green" />
              <span className="text-xs text-text-secondary">Disk Usage</span>
            </div>
            <p className="text-2xl font-bold text-text-primary">67.8%</p>
            <div className="flex items-center mt-2 text-sm text-status-success">
              <TrendingUp className="w-4 h-4 mr-1" />
              <span>+1.8%</span>
            </div>
          </div>

          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <div className="flex items-center justify-between mb-4">
              <Network className="w-5 h-5 text-accent-orange" />
              <span className="text-xs text-text-secondary">Network I/O</span>
            </div>
            <p className="text-2xl font-bold text-text-primary">1.2 GB/s</p>
            <div className="flex items-center mt-2 text-sm text-status-error">
              <TrendingUp className="w-4 h-4 mr-1" />
              <span>+8.4%</span>
            </div>
          </div>
        </div>

        {/* Charts */}
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          {/* CPU Usage Chart */}
          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <div className="flex items-center justify-between mb-4">
              <h2 className="text-lg font-semibold text-text-primary">CPU Usage Over Time</h2>
              <LineChart className="w-5 h-5 text-text-secondary" />
            </div>
            <div className="h-64 flex items-center justify-center bg-background-light/50 rounded-lg">
              <div className="text-center">
                <LineChart className="w-12 h-12 text-text-secondary mx-auto mb-2" />
                <p className="text-text-secondary">Line chart visualization</p>
                <p className="text-sm text-text-muted mt-1">24-hour CPU usage trend</p>
              </div>
            </div>
          </div>

          {/* Memory Usage Chart */}
          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <div className="flex items-center justify-between mb-4">
              <h2 className="text-lg font-semibold text-text-primary">Memory Usage Over Time</h2>
              <BarChart3 className="w-5 h-5 text-text-secondary" />
            </div>
            <div className="h-64 flex items-center justify-center bg-background-light/50 rounded-lg">
              <div className="text-center">
                <BarChart3 className="w-12 h-12 text-text-secondary mx-auto mb-2" />
                <p className="text-text-secondary">Bar chart visualization</p>
                <p className="text-sm text-text-muted mt-1">24-hour memory usage trend</p>
              </div>
            </div>
          </div>

          {/* Asset Distribution */}
          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <div className="flex items-center justify-between mb-4">
              <h2 className="text-lg font-semibold text-text-primary">Asset Distribution</h2>
              <PieChart className="w-5 h-5 text-text-secondary" />
            </div>
            <div className="h-64 flex items-center justify-center bg-background-light/50 rounded-lg">
              <div className="text-center">
                <PieChart className="w-12 h-12 text-text-secondary mx-auto mb-2" />
                <p className="text-text-secondary">Pie chart visualization</p>
                <p className="text-sm text-text-muted mt-1">Assets by type distribution</p>
              </div>
            </div>
          </div>

          {/* Network Traffic */}
          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <div className="flex items-center justify-between mb-4">
              <h2 className="text-lg font-semibold text-text-primary">Network Traffic</h2>
              <Network className="w-5 h-5 text-text-secondary" />
            </div>
            <div className="h-64 flex items-center justify-center bg-background-light/50 rounded-lg">
              <div className="text-center">
                <Network className="w-12 h-12 text-text-secondary mx-auto mb-2" />
                <p className="text-text-secondary">Area chart visualization</p>
                <p className="text-sm text-text-muted mt-1">Network in/out traffic</p>
              </div>
            </div>
          </div>
        </div>

        {/* Detailed Metrics Table */}
        <div className="bg-background-medium rounded-lg p-6 border border-border-color">
          <h2 className="text-lg font-semibold text-text-primary mb-4">Detailed Metrics</h2>
          <div className="overflow-x-auto">
            <table className="w-full">
              <thead>
                <tr className="border-b border-border-color">
                  <th className="px-4 py-3 text-left text-xs font-medium text-text-secondary uppercase tracking-wider">
                    Metric
                  </th>
                  <th className="px-4 py-3 text-left text-xs font-medium text-text-secondary uppercase tracking-wider">
                    Current
                  </th>
                  <th className="px-4 py-3 text-left text-xs font-medium text-text-secondary uppercase tracking-wider">
                    Average
                  </th>
                  <th className="px-4 py-3 text-left text-xs font-medium text-text-secondary uppercase tracking-wider">
                    Peak
                  </th>
                  <th className="px-4 py-3 text-left text-xs font-medium text-text-secondary uppercase tracking-wider">
                    Trend
                  </th>
                </tr>
              </thead>
              <tbody className="divide-y divide-border-color">
                <tr>
                  <td className="px-4 py-3 text-sm text-text-primary">CPU Usage</td>
                  <td className="px-4 py-3 text-sm text-text-primary">{formatPercentage(65.2)}</td>
                  <td className="px-4 py-3 text-sm text-text-primary">{formatPercentage(stats?.avgCpuUsage || 0)}</td>
                  <td className="px-4 py-3 text-sm text-text-primary">{formatPercentage(89.5)}</td>
                  <td className="px-4 py-3 text-sm text-status-success">+5.2%</td>
                </tr>
                <tr>
                  <td className="px-4 py-3 text-sm text-text-primary">Memory Usage</td>
                  <td className="px-4 py-3 text-sm text-text-primary">{formatPercentage(71.8)}</td>
                  <td className="px-4 py-3 text-sm text-text-primary">{formatPercentage(stats?.avgMemoryUsage || 0)}</td>
                  <td className="px-4 py-3 text-sm text-text-primary">{formatPercentage(85.2)}</td>
                  <td className="px-4 py-3 text-sm text-status-warning">+2.1%</td>
                </tr>
                <tr>
                  <td className="px-4 py-3 text-sm text-text-primary">Disk Usage</td>
                  <td className="px-4 py-3 text-sm text-text-primary">{formatPercentage(67.8)}</td>
                  <td className="px-4 py-3 text-sm text-text-primary">{formatPercentage(65.4)}</td>
                  <td className="px-4 py-3 text-sm text-text-primary">{formatPercentage(78.9)}</td>
                  <td className="px-4 py-3 text-sm text-status-success">+1.8%</td>
                </tr>
                <tr>
                  <td className="px-4 py-3 text-sm text-text-primary">Network In</td>
                  <td className="px-4 py-3 text-sm text-text-primary">1.2 GB/s</td>
                  <td className="px-4 py-3 text-sm text-text-primary">0.9 GB/s</td>
                  <td className="px-4 py-3 text-sm text-text-primary">2.5 GB/s</td>
                  <td className="px-4 py-3 text-sm text-status-error">+8.4%</td>
                </tr>
                <tr>
                  <td className="px-4 py-3 text-sm text-text-primary">Network Out</td>
                  <td className="px-4 py-3 text-sm text-text-primary">0.8 GB/s</td>
                  <td className="px-4 py-3 text-sm text-text-primary">0.6 GB/s</td>
                  <td className="px-4 py-3 text-sm text-text-primary">1.8 GB/s</td>
                  <td className="px-4 py-3 text-sm text-status-success">+3.2%</td>
                </tr>
              </tbody>
            </table>
          </div>
        </div>
      </div>
    </Layout>
  );
}
