#include <stdafx.h>
#include "CUploadBuffer.h"

CUploadBuffer::CUploadBuffer(const size_t bufferSize, CDescriptorHeap* descHeap,
                             ID3D12Device2* device,
							const CD3DX12_RESOURCE_DESC& bufferDesc)
{
	m_bufferSize = bufferSize;
	const auto type = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const auto typeUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const auto bufferUpload = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
	ThrowIfFailed(device->CreateCommittedResource(
		&type,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		nullptr,
		IID_PPV_ARGS(&m_myDefaultHeap)));

	ThrowIfFailed(device->CreateCommittedResource(
		&typeUpload,
		D3D12_HEAP_FLAG_NONE,
		&bufferUpload,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_myUploadHeap)));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = m_myUploadHeap->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = bufferSize; // CB size is required to be 256-byte aligned.
	device->CreateConstantBufferView(&cbvDesc, descHeap->GetCPUHandle());
	
	

	CD3DX12_RANGE readRange(0, 0);
	// We do not intend to read from this resource on the CPU. (End is less than or equal to begin)
	m_myUploadHeap->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedData));
}


bool CUploadBuffer::SubmitData(const void* bData, const size_t dataSize,
                               ID3D12GraphicsCommandList2* commandList)
{
	if(dataSize <= m_bufferSize)
	{
		const auto copyTransition = CD3DX12_RESOURCE_BARRIER::Transition(m_myDefaultHeap.Get(),
                                                                        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
                                                                        D3D12_RESOURCE_STATE_COPY_DEST);
		commandList->ResourceBarrier(1,
                                  &copyTransition);
		D3D12_SUBRESOURCE_DATA bufferData = {};
		bufferData.pData = bData;
		bufferData.RowPitch = dataSize;
		bufferData.SlicePitch = dataSize;

		UpdateSubresources(commandList, m_myDefaultHeap.Get(), m_myUploadHeap.Get(), 0, 0, 1, &bufferData);

		const auto bufferTransition = CD3DX12_RESOURCE_BARRIER::Transition(m_myDefaultHeap.Get(),
                                                                           D3D12_RESOURCE_STATE_COPY_DEST,
                                                                           D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		commandList->ResourceBarrier(1,
                                     &bufferTransition);
		return true;
	}
	return false;
}
