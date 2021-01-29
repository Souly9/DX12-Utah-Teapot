#include <stdafx.h>
#include "CDescriptorHeap.h"

CDescriptorHeap::CDescriptorHeap(UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type,
                                 D3D12_DESCRIPTOR_HEAP_FLAGS flags,
                                 ID3D12Device2* device)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;
	desc.Flags = flags;
	ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_descHeap)));
}

D3D12_CPU_DESCRIPTOR_HANDLE CDescriptorHeap::GetCPUHandle()
{
	return m_descHeap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_GPU_DESCRIPTOR_HANDLE CDescriptorHeap::GetGPUHandle()
{
	return m_descHeap->GetGPUDescriptorHandleForHeapStart();
}
