import { useQuery } from '@tanstack/react-query';
import { getDashboardStats, getActivities, getTimeSeriesData } from '@/lib/api';
import type { DashboardStats, Activity, TimeSeriesData } from '@/types';

export function useDashboardStats() {
  return useQuery({
    queryKey: ['dashboard-stats'],
    queryFn: getDashboardStats,
    staleTime: 1 * 60 * 1000, // 1 minute
    refetchInterval: 5 * 60 * 1000, // Auto-refresh every 5 minutes
  });
}

export function useActivities(limit = 10) {
  return useQuery({
    queryKey: ['activities', limit],
    queryFn: () => getActivities(limit),
    staleTime: 2 * 60 * 1000, // 2 minutes
    refetchInterval: 3 * 60 * 1000, // Auto-refresh every 3 minutes
  });
}

export function useTimeSeriesData(metric: string, hours = 24) {
  return useQuery({
    queryKey: ['timeseries', metric, hours],
    queryFn: () => getTimeSeriesData(metric, hours),
    staleTime: 5 * 60 * 1000, // 5 minutes
    refetchInterval: 1 * 60 * 1000, // Auto-refresh every minute
  });
}
