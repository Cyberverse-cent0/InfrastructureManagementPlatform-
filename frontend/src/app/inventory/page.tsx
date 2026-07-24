'use client';

import { useState } from 'react';
import { Layout } from '@/components/layout/Layout';
import { useAssets } from '@/hooks/useAssets';
import { useCreateAsset, useUpdateAsset, useDeleteAsset } from '@/hooks/useAssets';
import { useGlobalStore } from '@/store/globalStore';
import { formatRelativeTime, getStatusColor, getPriorityColor, cn } from '@/lib/utils';
import type { Asset, AssetFormData } from '@/types';
import {
  Search,
  Filter,
  Plus,
  Edit,
  Trash2,
  MoreVertical,
  ChevronDown,
  RefreshCw,
  Download,
  Upload
} from 'lucide-react';
import Button from '@/components/ui/Button';
import Badge from '@/components/ui/Badge';

export default function InventoryPage() {
  const { data, isLoading } = useAssets();
  const createAsset = useCreateAsset();
  const updateAsset = useUpdateAsset();
  const deleteAsset = useDeleteAsset();
  const selectedAssets = useGlobalStore((state) => state.selectedAssets);
  const toggleAssetSelection = useGlobalStore((state) => state.toggleAssetSelection);
  const clearSelectedAssets = useGlobalStore((state) => state.clearSelectedAssets);
  const [searchQuery, setSearchQuery] = useState('');
  const [statusFilter, setStatusFilter] = useState<string>('all');
  const [typeFilter, setTypeFilter] = useState<string>('all');

  const filteredAssets = data?.data.filter(asset => {
    const matchesSearch = asset.name.toLowerCase().includes(searchQuery.toLowerCase()) ||
                         asset.location.toLowerCase().includes(searchQuery.toLowerCase());
    const matchesStatus = statusFilter === 'all' || asset.status === statusFilter;
    const matchesType = typeFilter === 'all' || asset.type === typeFilter;
    return matchesSearch && matchesStatus && matchesType;
  }) || [];

  const handleSelectAll = () => {
    if (selectedAssets.size === filteredAssets.length) {
      clearSelectedAssets();
    } else {
      filteredAssets.forEach(asset => toggleAssetSelection(asset.id));
    }
  };

  const handleDelete = async (id: string) => {
    if (confirm('Are you sure you want to delete this asset?')) {
      await deleteAsset.mutateAsync(id);
    }
  };

  if (isLoading) {
    return (
      <Layout>
        <div className="flex items-center justify-center h-64">
          <RefreshCw className="w-8 h-8 text-text-secondary animate-spin" />
        </div>
      </Layout>
    );
  }

  return (
    <Layout>
      <div className="space-y-6">
        {/* Header */}
        <div className="flex items-center justify-between">
          <div>
            <h1 className="text-3xl font-bold text-text-primary">Inventory</h1>
            <p className="text-text-secondary mt-1">Manage your infrastructure assets</p>
          </div>
          <div className="flex space-x-2">
            <Button variant="secondary">
              <Upload className="w-4 h-4 mr-2" />
              Import
            </Button>
            <Button variant="secondary">
              <Download className="w-4 h-4 mr-2" />
              Export
            </Button>
            <Button>
              <Plus className="w-4 h-4 mr-2" />
              Add Asset
            </Button>
          </div>
        </div>

        {/* Filters */}
        <div className="bg-background-medium rounded-lg p-4 border border-border-color">
          <div className="flex flex-col md:flex-row gap-4">
            {/* Search */}
            <div className="relative flex-1">
              <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 w-4 h-4 text-text-secondary" />
              <input
                type="text"
                placeholder="Search assets..."
                value={searchQuery}
                onChange={(e) => setSearchQuery(e.target.value)}
                className="w-full pl-10 pr-4 py-2 bg-background-light border border-border-color rounded-lg text-sm text-text-primary placeholder-text-secondary focus:outline-none focus:ring-2 focus:ring-accent-blue"
              />
            </div>

            {/* Status Filter */}
            <select
              value={statusFilter}
              onChange={(e) => setStatusFilter(e.target.value)}
              className="px-4 py-2 bg-background-light border border-border-color rounded-lg text-sm text-text-primary focus:outline-none focus:ring-2 focus:ring-accent-blue"
            >
              <option value="all">All Status</option>
              <option value="active">Active</option>
              <option value="inactive">Inactive</option>
              <option value="maintenance">Maintenance</option>
              <option value="error">Error</option>
            </select>

            {/* Type Filter */}
            <select
              value={typeFilter}
              onChange={(e) => setTypeFilter(e.target.value)}
              className="px-4 py-2 bg-background-light border border-border-color rounded-lg text-sm text-text-primary focus:outline-none focus:ring-2 focus:ring-accent-blue"
            >
              <option value="all">All Types</option>
              <option value="server">Servers</option>
              <option value="database">Databases</option>
              <option value="storage">Storage</option>
              <option value="network">Network</option>
              <option value="load-balancer">Load Balancers</option>
            </select>

            {/* Bulk Actions */}
            {selectedAssets.size > 0 && (
              <div className="flex items-center space-x-2">
                <span className="text-sm text-text-secondary">
                  {selectedAssets.size} selected
                </span>
                <Button variant="danger" size="sm">
                  <Trash2 className="w-4 h-4 mr-2" />
                  Delete Selected
                </Button>
              </div>
            )}
          </div>
        </div>

        {/* Data Table */}
        <div className="bg-background-medium rounded-lg border border-border-color overflow-hidden">
          <div className="overflow-x-auto">
            <table className="w-full">
              <thead className="bg-background-light">
                <tr>
                  <th className="px-4 py-3 text-left">
                    <input
                      type="checkbox"
                      checked={selectedAssets.size === filteredAssets.length && filteredAssets.length > 0}
                      onChange={handleSelectAll}
                      className="rounded border-border-color"
                    />
                  </th>
                  <th className="px-4 py-3 text-left text-xs font-medium text-text-secondary uppercase tracking-wider">
                    Name
                  </th>
                  <th className="px-4 py-3 text-left text-xs font-medium text-text-secondary uppercase tracking-wider">
                    Type
                  </th>
                  <th className="px-4 py-3 text-left text-xs font-medium text-text-secondary uppercase tracking-wider">
                    Status
                  </th>
                  <th className="px-4 py-3 text-left text-xs font-medium text-text-secondary uppercase tracking-wider">
                    Priority
                  </th>
                  <th className="px-4 py-3 text-left text-xs font-medium text-text-secondary uppercase tracking-wider">
                    Location
                  </th>
                  <th className="px-4 py-3 text-left text-xs font-medium text-text-secondary uppercase tracking-wider">
                    Last Updated
                  </th>
                  <th className="px-4 py-3 text-left text-xs font-medium text-text-secondary uppercase tracking-wider">
                    Actions
                  </th>
                </tr>
              </thead>
              <tbody className="divide-y divide-border-color">
                {filteredAssets.map((asset) => (
                  <tr key={asset.id} className="hover:bg-background-light/50 transition-colors">
                    <td className="px-4 py-3">
                      <input
                        type="checkbox"
                        checked={selectedAssets.has(asset.id)}
                        onChange={() => toggleAssetSelection(asset.id)}
                        className="rounded border-border-color"
                      />
                    </td>
                    <td className="px-4 py-3">
                      <div>
                        <button 
                          onClick={() => router.push(`/inventory/${asset.id}`)}
                          className="text-sm font-medium text-text-primary hover:text-accent-blue transition-colors"
                        >
                          {asset.name}
                        </button>
                        <div className="text-xs text-text-secondary">{asset.ipAddress || 'No IP'}</div>
                      </div>
                    </td>
                    <td className="px-4 py-3">
                      <Badge variant="default">{asset.type}</Badge>
                    </td>
                    <td className="px-4 py-3">
                      <span className={cn('px-2 py-1 rounded-full text-xs font-medium', getStatusColor(asset.status))}>
                        {asset.status}
                      </span>
                    </td>
                    <td className="px-4 py-3">
                      <span className={cn('px-2 py-1 rounded-full text-xs font-medium', getPriorityColor(asset.priority))}>
                        {asset.priority}
                      </span>
                    </td>
                    <td className="px-4 py-3 text-sm text-text-secondary">
                      {asset.location}
                    </td>
                    <td className="px-4 py-3 text-sm text-text-secondary">
                      {formatRelativeTime(asset.updatedAt)}
                    </td>
                    <td className="px-4 py-3">
                      <div className="flex items-center space-x-2">
                        <Button 
                          variant="ghost" 
                          size="sm"
                          onClick={() => router.push(`/inventory/${asset.id}`)}
                        >
                          <Edit className="w-4 h-4" />
                        </Button>
                        <Button 
                          variant="ghost" 
                          size="sm"
                          onClick={() => handleDelete(asset.id)}
                        >
                          <Trash2 className="w-4 h-4 text-status-error" />
                        </Button>
                      </div>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>

          {/* Pagination */}
          <div className="px-4 py-3 bg-background-light border-t border-border-color flex items-center justify-between">
            <div className="text-sm text-text-secondary">
              Showing {filteredAssets.length} of {data?.pagination.total || 0} assets
            </div>
            <div className="flex items-center space-x-2">
              <Button variant="ghost" size="sm" disabled>
                Previous
              </Button>
              <span className="text-sm text-text-secondary">
                Page {data?.pagination.page || 1} of {Math.ceil((data?.pagination.total || 0) / (data?.pagination.limit || 10))}
              </span>
              <Button variant="ghost" size="sm">
                Next
              </Button>
            </div>
          </div>
        </div>
      </div>
    </Layout>
  );
}
