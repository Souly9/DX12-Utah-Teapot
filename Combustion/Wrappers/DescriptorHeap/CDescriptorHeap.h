#pragma once
#include <stdafx.h>
/**
 * Simple wrapper class for DescriptorHeaps
 */
class CDescriptorHeap
{
public:
	CDescriptorHeap(UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type,
	                D3D12_DESCRIPTOR_HEAP_FLAGS flags,
	                ID3D12Device2* device);
	
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetHeap(){return m_descHeap;};
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descHeap;
};
