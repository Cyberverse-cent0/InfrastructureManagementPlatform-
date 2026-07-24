// Core Types for Inventory Management System
export * from './auth';

export type AssetType = 'server' | 'network' | 'storage' | 'database' | 'load-balancer';
export type AssetStatus = 'active' | 'inactive' | 'maintenance' | 'error' | 'provisioning';
export type Priority = 'low' | 'medium' | 'high' | 'critical';

export interface Asset {
  id: string;
  name: string;
  type: AssetType;
  status: AssetStatus;
  priority: Priority;
  location: string;
  cpu?: number;
  memory?: number;
  storage?: number;
  ipAddress?: string;
  macAddress?: string;
  operatingSystem?: string;
  createdAt: Date;
  updatedAt: Date;
  lastChecked: Date;
  tags: string[];
  notes?: string;
}

export interface ServerMetrics {
  assetId: string;
  timestamp: Date;
  cpuUsage: number;
  memoryUsage: number;
  diskUsage: number;
  networkIn: number;
  networkOut: number;
  activeConnections: number;
}

export interface DashboardStats {
  totalAssets: number;
  activeAssets: number;
  inactiveAssets: number;
  maintenanceAssets: number;
  errorAssets: number;
  totalServers: number;
  totalNetwork: number;
  totalStorage: number;
  totalDatabases: number;
  avgCpuUsage: number;
  avgMemoryUsage: number;
  uptime: number;
}

export interface Activity {
  id: string;
  type: 'create' | 'update' | 'delete' | 'error' | 'maintenance';
  assetId?: string;
  assetName?: string;
  userId: string;
  userName: string;
  message: string;
  timestamp: Date;
  metadata?: Record<string, any>;
}

export interface FilterOptions {
  type?: AssetType[];
  status?: AssetStatus[];
  priority?: Priority[];
  location?: string[];
  searchQuery?: string;
  dateRange?: {
    start: Date;
    end: Date;
  };
}

export interface SortOptions {
  field: keyof Asset;
  direction: 'asc' | 'desc';
}

export interface PaginationOptions {
  page: number;
  limit: number;
  total: number;
}

export interface ApiResponse<T> {
  data: T;
  success: boolean;
  message?: string;
  error?: string;
}

export interface PaginatedResponse<T> {
  data: T[];
  pagination: PaginationOptions;
  filters: FilterOptions;
  sort: SortOptions;
}

// Form Types
export interface AssetFormData {
  name: string;
  type: AssetType;
  status: AssetStatus;
  priority: Priority;
  location: string;
  cpu?: number;
  memory?: number;
  storage?: number;
  ipAddress?: string;
  macAddress?: string;
  operatingSystem?: string;
  tags: string[];
  notes?: string;
}

export interface ChartData {
  name: string;
  value: number;
  [key: string]: string | number;
}

export interface TimeSeriesData {
  timestamp: Date;
  value: number;
  label?: string;
}

export interface Notification {
  id: string;
  type: 'success' | 'warning' | 'error' | 'info';
  title: string;
  message: string;
  timestamp: Date;
  read: boolean;
  action?: {
    label: string;
    callback: () => void;
  };
}

export interface UserPreferences {
  theme: 'dark' | 'light';
  language: string;
  dashboardLayout: 'grid' | 'list';
  defaultView: 'dashboard' | 'inventory' | 'analytics';
  notifications: boolean;
  autoRefresh: boolean;
  refreshInterval: number;
}

export interface SidebarItem {
  id: string;
  label: string;
  icon: string;
  path: string;
  badge?: number;
  children?: SidebarItem[];
}

export interface BreadcrumbItem {
  label: string;
  path?: string;
}
