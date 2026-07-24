import type { 
  Asset, 
  DashboardStats, 
  Activity, 
  FilterOptions, 
  SortOptions, 
  PaginatedResponse,
  AssetFormData,
  ServerMetrics,
  TimeSeriesData
} from '@/types';

// API Configuration
const API_BASE_URL = process.env.NEXT_PUBLIC_API_URL || 'http://localhost:8080';

// Helper function for API calls
async function apiCall<T>(endpoint: string, options: RequestInit = {}): Promise<T> {
  const url = `${API_BASE_URL}${endpoint}`;
  
  const defaultOptions: RequestInit = {
    headers: {
      'Content-Type': 'application/json',
      ...options.headers,
    },
    ...options,
  };

  try {
    const response = await fetch(url, defaultOptions);
    
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    
    return await response.json();
  } catch (error) {
    console.error(`API call failed for ${endpoint}:`, error);
    throw error;
  }
}

// Mock data fallback for development
const mockAssets: Asset[] = [
  {
    id: '1',
    name: 'Web Server 01',
    type: 'server',
    status: 'active',
    priority: 'high',
    location: 'US-East-1',
    cpu: 4,
    memory: 16,
    storage: 500,
    ipAddress: '192.168.1.100',
    macAddress: '00:1A:2B:3C:4D:5E',
    operatingSystem: 'Ubuntu 22.04',
    createdAt: new Date('2024-01-15'),
    updatedAt: new Date('2024-07-20'),
    lastChecked: new Date(),
    tags: ['web', 'production'],
    notes: 'Main web server for production traffic'
  },
  {
    id: '2',
    name: 'Database Server 01',
    type: 'database',
    status: 'active',
    priority: 'critical',
    location: 'US-East-1',
    cpu: 8,
    memory: 32,
    storage: 1000,
    ipAddress: '192.168.1.101',
    macAddress: '00:1A:2B:3C:4D:5F',
    operatingSystem: 'CentOS 8',
    createdAt: new Date('2024-02-01'),
    updatedAt: new Date('2024-07-22'),
    lastChecked: new Date(),
    tags: ['database', 'production', 'critical'],
    notes: 'Primary PostgreSQL database server'
  },
  {
    id: '3',
    name: 'Load Balancer 01',
    type: 'load-balancer',
    status: 'active',
    priority: 'high',
    location: 'US-West-2',
    cpu: 2,
    memory: 8,
    ipAddress: '192.168.2.100',
    macAddress: '00:1A:2B:3C:4D:60',
    operatingSystem: 'Ubuntu 22.04',
    createdAt: new Date('2024-03-10'),
    updatedAt: new Date('2024-07-23'),
    lastChecked: new Date(),
    tags: ['network', 'production'],
    notes: 'HAProxy load balancer'
  },
  {
    id: '4',
    name: 'Storage Server 01',
    type: 'storage',
    status: 'maintenance',
    priority: 'medium',
    location: 'EU-Central-1',
    cpu: 4,
    memory: 16,
    storage: 2000,
    ipAddress: '192.168.3.100',
    macAddress: '00:1A:2B:3C:4D:61',
    operatingSystem: 'Debian 11',
    createdAt: new Date('2024-04-05'),
    updatedAt: new Date('2024-07-24'),
    lastChecked: new Date(),
    tags: ['storage', 'backup'],
    notes: 'NAS storage server for backups'
  },
  {
    id: '5',
    name: 'Dev Server 01',
    type: 'server',
    status: 'inactive',
    priority: 'low',
    location: 'US-East-1',
    cpu: 2,
    memory: 8,
    storage: 250,
    ipAddress: '192.168.1.102',
    macAddress: '00:1A:2B:3C:4D:62',
    operatingSystem: 'Ubuntu 22.04',
    createdAt: new Date('2024-05-15'),
    updatedAt: new Date('2024-07-10'),
    lastChecked: new Date(),
    tags: ['development', 'testing'],
    notes: 'Development and testing server'
  }
];

const mockActivities: Activity[] = [
  {
    id: '1',
    type: 'create',
    assetId: '5',
    assetName: 'Dev Server 01',
    userId: 'user1',
    userName: 'John Doe',
    message: 'Created new asset Dev Server 01',
    timestamp: new Date('2024-07-24T10:30:00'),
  },
  {
    id: '2',
    type: 'update',
    assetId: '4',
    assetName: 'Storage Server 01',
    userId: 'user2',
    userName: 'Jane Smith',
    message: 'Updated Storage Server 01 status to maintenance',
    timestamp: new Date('2024-07-24T09:15:00'),
  },
  {
    id: '3',
    type: 'error',
    assetId: '2',
    assetName: 'Database Server 01',
    userId: 'system',
    userName: 'System',
    message: 'High CPU usage detected on Database Server 01',
    timestamp: new Date('2024-07-24T08:00:00'),
  },
  {
    id: '4',
    type: 'maintenance',
    assetId: '1',
    assetName: 'Web Server 01',
    userId: 'user1',
    userName: 'John Doe',
    message: 'Scheduled maintenance completed for Web Server 01',
    timestamp: new Date('2024-07-23T16:00:00'),
  }
];

const mockMetrics: ServerMetrics[] = Array.from({ length: 24 }, (_, i) => ({
  assetId: '1',
  timestamp: new Date(Date.now() - (23 - i) * 3600000),
  cpuUsage: Math.random() * 30 + 40,
  memoryUsage: Math.random() * 20 + 60,
  diskUsage: Math.random() * 10 + 50,
  networkIn: Math.random() * 1000,
  networkOut: Math.random() * 800,
  activeConnections: Math.floor(Math.random() * 100 + 50),
}));

// Flag to use mock data or real API
const USE_MOCK_DATA = true; // Set to false to use real API

// API Functions (with fallback to mock data)
export async function getAssets(
  filters?: FilterOptions,
  sort?: SortOptions,
  page = 1,
  limit = 10
): Promise<PaginatedResponse<Asset>> {
  if (USE_MOCK_DATA) {
    await new Promise(resolve => setTimeout(resolve, 500)); // Simulate network delay
    
    let filteredAssets = [...mockAssets];
    
    // Apply filters
    if (filters?.type?.length) {
      filteredAssets = filteredAssets.filter(asset => 
        filters.type!.includes(asset.type)
      );
    }
    
    if (filters?.status?.length) {
      filteredAssets = filteredAssets.filter(asset => 
        filters.status!.includes(asset.status)
      );
    }
    
    if (filters?.searchQuery) {
      const query = filters.searchQuery.toLowerCase();
      filteredAssets = filteredAssets.filter(asset =>
        asset.name.toLowerCase().includes(query) ||
        asset.location.toLowerCase().includes(query) ||
        asset.tags.some(tag => tag.toLowerCase().includes(query))
      );
    }
    
    // Apply sorting
    if (sort) {
      filteredAssets.sort((a, b) => {
        const aValue = a[sort.field];
        const bValue = b[sort.field];
        const comparison = aValue < bValue ? -1 : aValue > bValue ? 1 : 0;
        return sort.direction === 'asc' ? comparison : -comparison;
      });
    }
    
    // Apply pagination
    const total = filteredAssets.length;
    const startIndex = (page - 1) * limit;
    const paginatedAssets = filteredAssets.slice(startIndex, startIndex + limit);
    
    return {
      data: paginatedAssets,
      pagination: {
        page,
        limit,
        total,
      },
      filters: filters || {},
      sort: sort || { field: 'name', direction: 'asc' },
    };
  }

  try {
    const response = await apiCall<any>('/api/assets');
    return {
      data: response.data || [],
      pagination: response.pagination || { page, limit, total: response.data?.length || 0 },
      filters: filters || {},
      sort: sort || { field: 'name', direction: 'asc' },
    };
  } catch (error) {
    console.error('Failed to fetch assets from API, falling back to mock data');
    // Fallback to mock data on API failure
    return getAssets(filters, sort, page, limit);
  }
}

export async function getAsset(id: string): Promise<Asset | null> {
  if (USE_MOCK_DATA) {
    await new Promise(resolve => setTimeout(resolve, 300));
    return mockAssets.find(asset => asset.id === id) || null;
  }

  try {
    return await apiCall<Asset>(`/api/assets/${id}`);
  } catch (error) {
    console.error('Failed to fetch asset from API, falling back to mock data');
    return mockAssets.find(asset => asset.id === id) || null;
  }
}

export async function createAsset(data: AssetFormData): Promise<Asset> {
  if (USE_MOCK_DATA) {
    await new Promise(resolve => setTimeout(resolve, 500));
    
    const newAsset: Asset = {
      id: Date.now().toString(),
      ...data,
      createdAt: new Date(),
      updatedAt: new Date(),
      lastChecked: new Date(),
    };
    
    mockAssets.push(newAsset);
    return newAsset;
  }

  try {
    return await apiCall<Asset>('/api/assets', {
      method: 'POST',
      body: JSON.stringify(data),
    });
  } catch (error) {
    console.error('Failed to create asset via API');
    throw error;
  }
}

export async function updateAsset(id: string, data: Partial<AssetFormData>): Promise<Asset | null> {
  if (USE_MOCK_DATA) {
    await new Promise(resolve => setTimeout(resolve, 500));
    
    const index = mockAssets.findIndex(asset => asset.id === id);
    if (index === -1) return null;
    
    mockAssets[index] = {
      ...mockAssets[index],
      ...data,
      updatedAt: new Date(),
    };
    
    return mockAssets[index];
  }

  try {
    return await apiCall<Asset>(`/api/assets/${id}`, {
      method: 'PUT',
      body: JSON.stringify(data),
    });
  } catch (error) {
    console.error('Failed to update asset via API');
    throw error;
  }
}

export async function deleteAsset(id: string): Promise<boolean> {
  if (USE_MOCK_DATA) {
    await new Promise(resolve => setTimeout(resolve, 500));
    
    const index = mockAssets.findIndex(asset => asset.id === id);
    if (index === -1) return false;
    
    mockAssets.splice(index, 1);
    return true;
  }

  try {
    await apiCall<void>(`/api/assets/${id}`, {
      method: 'DELETE',
    });
    return true;
  } catch (error) {
    console.error('Failed to delete asset via API');
    throw error;
  }
}

export async function getDashboardStats(): Promise<DashboardStats> {
  if (USE_MOCK_DATA) {
    await new Promise(resolve => setTimeout(resolve, 300));
    
    return {
      totalAssets: mockAssets.length,
      activeAssets: mockAssets.filter(a => a.status === 'active').length,
      inactiveAssets: mockAssets.filter(a => a.status === 'inactive').length,
      maintenanceAssets: mockAssets.filter(a => a.status === 'maintenance').length,
      errorAssets: mockAssets.filter(a => a.status === 'error').length,
      totalServers: mockAssets.filter(a => a.type === 'server').length,
      totalNetwork: mockAssets.filter(a => a.type === 'network').length,
      totalStorage: mockAssets.filter(a => a.type === 'storage').length,
      totalDatabases: mockAssets.filter(a => a.type === 'database').length,
      avgCpuUsage: 65.5,
      avgMemoryUsage: 72.3,
      uptime: 99.9,
    };
  }

  try {
    return await apiCall<DashboardStats>('/api/stats');
  } catch (error) {
    console.error('Failed to fetch stats from API, falling back to mock data');
    return getDashboardStats();
  }
}

export async function getActivities(limit = 10): Promise<Activity[]> {
  if (USE_MOCK_DATA) {
    await new Promise(resolve => setTimeout(resolve, 300));
    return mockActivities.slice(0, limit);
  }

  try {
    return await apiCall<Activity[]>('/api/activities');
  } catch (error) {
    console.error('Failed to fetch activities from API, falling back to mock data');
    return mockActivities.slice(0, limit);
  }
}

export async function getAssetMetrics(assetId: string, hours = 24): Promise<ServerMetrics[]> {
  if (USE_MOCK_DATA) {
    await new Promise(resolve => setTimeout(resolve, 300));
    return mockMetrics.filter(m => m.assetId === assetId).slice(-hours);
  }

  try {
    return await apiCall<ServerMetrics[]>(`/api/assets/${assetId}/metrics?hours=${hours}`);
  } catch (error) {
    console.error('Failed to fetch metrics from API, falling back to mock data');
    return mockMetrics.filter(m => m.assetId === assetId).slice(-hours);
  }
}

export async function getTimeSeriesData(metric: string, hours = 24): Promise<TimeSeriesData[]> {
  if (USE_MOCK_DATA) {
    await new Promise(resolve => setTimeout(resolve, 300));
    
    return Array.from({ length: hours }, (_, i) => ({
      timestamp: new Date(Date.now() - (hours - 1 - i) * 3600000),
      value: Math.random() * 30 + 50,
      label: new Date(Date.now() - (hours - 1 - i) * 3600000).getHours() + ':00',
    }));
  }

  try {
    return await apiCall<TimeSeriesData[]>(`/api/metrics/${metric}?hours=${hours}`);
  } catch (error) {
    console.error('Failed to fetch time series data from API, falling back to mock data');
    return getTimeSeriesData(metric, hours);
  }
}
