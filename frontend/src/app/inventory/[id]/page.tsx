'use client';

import { useParams, useRouter } from 'next/navigation';
import { Layout } from '@/components/layout/Layout';
import { useAsset } from '@/hooks/useAssets';
import { formatDate, getStatusColor, getPriorityColor, cn } from '@/lib/utils';
import {
  ArrowLeft,
  Edit,
  Trash2,
  Server,
  Database,
  HardDrive,
  Network,
  Activity,
  Cpu,
  MemoryStick,
  HardDrive as StorageIcon,
  Globe,
  Calendar,
  Clock,
  Tag,
  FileText,
  Settings
} from 'lucide-react';
import Button from '@/components/ui/Button';
import Badge from '@/components/ui/Badge';

const typeIcons = {
  server: Server,
  database: Database,
  storage: HardDrive,
  network: Network,
  'load-balancer': Activity,
};

export default function AssetDetailPage() {
  const params = useParams();
  const router = useRouter();
  const { data: asset, isLoading } = useAsset(params.id as string);

  if (isLoading) {
    return (
      <Layout>
        <div className="flex items-center justify-center h-64">
          <Activity className="w-8 h-8 text-text-secondary animate-spin" />
        </div>
      </Layout>
    );
  }

  if (!asset) {
    return (
      <Layout>
        <div className="text-center py-12">
          <h2 className="text-2xl font-bold text-text-primary mb-2">Asset Not Found</h2>
          <p className="text-text-secondary mb-4">The asset you're looking for doesn't exist.</p>
          <Button onClick={() => router.push('/inventory')}>
            <ArrowLeft className="w-4 h-4 mr-2" />
            Back to Inventory
          </Button>
        </div>
      </Layout>
    );
  }

  const TypeIcon = typeIcons[asset.type] || Server;

  return (
    <Layout>
      <div className="space-y-6">
        {/* Header */}
        <div className="flex items-center justify-between">
          <div className="flex items-center space-x-4">
            <Button
              variant="ghost"
              size="sm"
              onClick={() => router.push('/inventory')}
            >
              <ArrowLeft className="w-4 h-4 mr-2" />
              Back
            </Button>
            <div>
              <h1 className="text-3xl font-bold text-text-primary">{asset.name}</h1>
              <p className="text-text-secondary mt-1">{asset.type} • {asset.location}</p>
            </div>
          </div>
          <div className="flex space-x-2">
            <Button variant="secondary">
              <Edit className="w-4 h-4 mr-2" />
              Edit
            </Button>
            <Button variant="danger">
              <Trash2 className="w-4 h-4 mr-2" />
              Delete
            </Button>
          </div>
        </div>

        {/* Status Cards */}
        <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <div className="flex items-center justify-between mb-2">
              <span className="text-text-secondary text-sm">Status</span>
              <Activity className="w-4 h-4 text-text-secondary" />
            </div>
            <span className={cn('px-3 py-1 rounded-full text-sm font-medium', getStatusColor(asset.status))}>
              {asset.status}
            </span>
          </div>

          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <div className="flex items-center justify-between mb-2">
              <span className="text-text-secondary text-sm">Priority</span>
              <Settings className="w-4 h-4 text-text-secondary" />
            </div>
            <span className={cn('px-3 py-1 rounded-full text-sm font-medium', getPriorityColor(asset.priority))}>
              {asset.priority}
            </span>
          </div>

          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <div className="flex items-center justify-between mb-2">
              <span className="text-text-secondary text-sm">Last Checked</span>
              <Clock className="w-4 h-4 text-text-secondary" />
            </div>
            <span className="text-text-primary font-medium">{formatDate(asset.lastChecked)}</span>
          </div>
        </div>

        {/* Details Grid */}
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          {/* Basic Information */}
          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <h2 className="text-lg font-semibold text-text-primary mb-4 flex items-center">
              <TypeIcon className="w-5 h-5 mr-2" />
              Basic Information
            </h2>
            <div className="space-y-4">
              <div className="flex items-start space-x-3">
                <Server className="w-5 h-5 text-text-secondary mt-0.5" />
                <div className="flex-1">
                  <p className="text-sm text-text-secondary">Asset ID</p>
                  <p className="text-text-primary font-medium">{asset.id}</p>
                </div>
              </div>

              <div className="flex items-start space-x-3">
                <Globe className="w-5 h-5 text-text-secondary mt-0.5" />
                <div className="flex-1">
                  <p className="text-sm text-text-secondary">IP Address</p>
                  <p className="text-text-primary font-medium">{asset.ipAddress || 'N/A'}</p>
                </div>
              </div>

              <div className="flex items-start space-x-3">
                <Globe className="w-5 h-5 text-text-secondary mt-0.5" />
                <div className="flex-1">
                  <p className="text-sm text-text-secondary">MAC Address</p>
                  <p className="text-text-primary font-medium">{asset.macAddress || 'N/A'}</p>
                </div>
              </div>

              <div className="flex items-start space-x-3">
                <Server className="w-5 h-5 text-text-secondary mt-0.5" />
                <div className="flex-1">
                  <p className="text-sm text-text-secondary">Operating System</p>
                  <p className="text-text-primary font-medium">{asset.operatingSystem || 'N/A'}</p>
                </div>
              </div>

              <div className="flex items-start space-x-3">
                <Calendar className="w-5 h-5 text-text-secondary mt-0.5" />
                <div className="flex-1">
                  <p className="text-sm text-text-secondary">Created</p>
                  <p className="text-text-primary font-medium">{formatDate(asset.createdAt)}</p>
                </div>
              </div>

              <div className="flex items-start space-x-3">
                <Clock className="w-5 h-5 text-text-secondary mt-0.5" />
                <div className="flex-1">
                  <p className="text-sm text-text-secondary">Last Updated</p>
                  <p className="text-text-primary font-medium">{formatDate(asset.updatedAt)}</p>
                </div>
              </div>
            </div>
          </div>

          {/* Resource Information */}
          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <h2 className="text-lg font-semibold text-text-primary mb-4 flex items-center">
              <Activity className="w-5 h-5 mr-2" />
              Resources
            </h2>
            <div className="space-y-4">
              {asset.cpu !== undefined && (
                <div className="flex items-start space-x-3">
                  <Cpu className="w-5 h-5 text-text-secondary mt-0.5" />
                  <div className="flex-1">
                    <p className="text-sm text-text-secondary">CPU Cores</p>
                    <p className="text-text-primary font-medium">{asset.cpu} vCPUs</p>
                  </div>
                </div>
              )}

              {asset.memory !== undefined && (
                <div className="flex items-start space-x-3">
                  <MemoryStick className="w-5 h-5 text-text-secondary mt-0.5" />
                  <div className="flex-1">
                    <p className="text-sm text-text-secondary">Memory</p>
                    <p className="text-text-primary font-medium">{asset.memory} GB RAM</p>
                  </div>
                </div>
              )}

              {asset.storage !== undefined && (
                <div className="flex items-start space-x-3">
                  <StorageIcon className="w-5 h-5 text-text-secondary mt-0.5" />
                  <div className="flex-1">
                    <p className="text-sm text-text-secondary">Storage</p>
                    <p className="text-text-primary font-medium">{asset.storage} GB</p>
                  </div>
                </div>
              )}
            </div>
          </div>
        </div>

        {/* Tags */}
        {asset.tags.length > 0 && (
          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <h2 className="text-lg font-semibold text-text-primary mb-4 flex items-center">
              <Tag className="w-5 h-5 mr-2" />
              Tags
            </h2>
            <div className="flex flex-wrap gap-2">
              {asset.tags.map((tag) => (
                <Badge key={tag} variant="default">
                  {tag}
                </Badge>
              ))}
            </div>
          </div>
        )}

        {/* Notes */}
        {asset.notes && (
          <div className="bg-background-medium rounded-lg p-6 border border-border-color">
            <h2 className="text-lg font-semibold text-text-primary mb-4 flex items-center">
              <FileText className="w-5 h-5 mr-2" />
              Notes
            </h2>
            <p className="text-text-secondary">{asset.notes}</p>
          </div>
        )}
      </div>
    </Layout>
  );
}
