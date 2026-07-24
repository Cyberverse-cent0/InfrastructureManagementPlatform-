import { useQuery, useMutation, useQueryClient } from '@tanstack/react-query';
import { getAssets, getAsset, createAsset, updateAsset, deleteAsset } from '@/lib/api';
import type { Asset, FilterOptions, SortOptions, AssetFormData } from '@/types';
import { useGlobalStore } from '@/store/globalStore';

export function useAssets() {
  const filters = useGlobalStore((state) => state.filters);
  const sort = useGlobalStore((state) => state.sort);
  
  return useQuery({
    queryKey: ['assets', filters, sort],
    queryFn: () => getAssets(filters, sort),
    staleTime: 5 * 60 * 1000, // 5 minutes
  });
}

export function useAsset(id: string) {
  return useQuery({
    queryKey: ['asset', id],
    queryFn: () => getAsset(id),
    enabled: !!id,
    staleTime: 2 * 60 * 1000, // 2 minutes
  });
}

export function useCreateAsset() {
  const queryClient = useQueryClient();
  const addNotification = useGlobalStore((state) => state.addNotification);
  
  return useMutation({
    mutationFn: (data: AssetFormData) => createAsset(data),
    onSuccess: (data) => {
      queryClient.invalidateQueries({ queryKey: ['assets'] });
      addNotification({
        type: 'success',
        title: 'Asset Created',
        message: `Successfully created ${data.name}`,
      });
    },
    onError: (error) => {
      addNotification({
        type: 'error',
        title: 'Creation Failed',
        message: `Failed to create asset: ${error.message}`,
      });
    },
  });
}

export function useUpdateAsset() {
  const queryClient = useQueryClient();
  const addNotification = useGlobalStore((state) => state.addNotification);
  
  return useMutation({
    mutationFn: ({ id, data }: { id: string; data: Partial<AssetFormData> }) =>
      updateAsset(id, data),
    onSuccess: (data) => {
      queryClient.invalidateQueries({ queryKey: ['assets'] });
      queryClient.invalidateQueries({ queryKey: ['asset', data.id] });
      addNotification({
        type: 'success',
        title: 'Asset Updated',
        message: `Successfully updated ${data.name}`,
      });
    },
    onError: (error) => {
      addNotification({
        type: 'error',
        title: 'Update Failed',
        message: `Failed to update asset: ${error.message}`,
      });
    },
  });
}

export function useDeleteAsset() {
  const queryClient = useQueryClient();
  const addNotification = useGlobalStore((state) => state.addNotification);
  
  return useMutation({
    mutationFn: (id: string) => deleteAsset(id),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: ['assets'] });
      addNotification({
        type: 'success',
        title: 'Asset Deleted',
        message: 'Successfully deleted asset',
      });
    },
    onError: (error) => {
      addNotification({
        type: 'error',
        title: 'Deletion Failed',
        message: `Failed to delete asset: ${error.message}`,
      });
    },
  });
}
