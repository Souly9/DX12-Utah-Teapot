#include <stdafx.h>
#include "ShaderResource.h"


ShaderResource::ShaderResource(ShaderResource& other)
{
	m_bufferSize = other.m_bufferSize;
	m_myDescHeap = other.m_myDescHeap;
	m_myDefaultHeap = other.m_myDefaultHeap;
	m_myUploadHeap = other.m_myUploadHeap;
}

ShaderResource::ShaderResource(const size_t bufferSize, CDescriptorHeap* descHeap,
                               ID3D12Device2* device,
                               const CD3DX12_RESOURCE_DESC& bufferDesc,
                               BufferType bufferType, DirectX::ScratchImage* image)
{
	size_t size = ValidateBufferSize(bufferSize);
	m_bufferSize = bufferSize;

	const auto type = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const auto typeUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const auto bufferUpload = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
	ThrowIfFailed(device->CreateCommittedResource(
		&type,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_myDefaultHeap)));

	ThrowIfFailed(device->CreateCommittedResource(
		&typeUpload,
		D3D12_HEAP_FLAG_NONE,
		&bufferUpload,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_myUploadHeap)));

	D3D12_SHADER_RESOURCE_VIEW_DESC shaderView = {};
	shaderView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	shaderView.Format = image->GetMetadata().format;
	shaderView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	shaderView.Texture2D.MipLevels = 1;
	const auto handle = descHeap->GetCPUHandle();
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle0(handle, 0,
	                                         device->GetDescriptorHandleIncrementSize(
		                                         D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	device->CreateShaderResourceView(m_myDefaultHeap.Get(), &shaderView, srvHandle0);
}

ShaderResource::ShaderResource()
{
}

ShaderResource::~ShaderResource()
{
}

bool ShaderResource::SubmitTexture(DirectX::ScratchImage* image, ID3D12GraphicsCommandList2* commandList)
{
	D3D12_SUBRESOURCE_DATA bufferData = {};
	bufferData.pData = image->GetPixels();
	bufferData.RowPitch = image->GetImages()->rowPitch;
	bufferData.SlicePitch = image->GetImages()->slicePitch;

	ThrowIfFailed(UpdateSubresources(commandList, m_myDefaultHeap.Get(), m_myUploadHeap.Get(), 0, 0, 1, &bufferData));

	const auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_myDefaultHeap.Get(),
	                                                             D3D12_RESOURCE_STATE_COPY_DEST,
	                                                             D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandList->ResourceBarrier(1,
	                             &transition);
	return true;
}

bool ShaderResource::SubmitResource(const void* bData, const size_t dataSize, ID3D12GraphicsCommandList2* commandList)
{
	return true;
}

constexpr size_t ShaderResource::ValidateBufferSize(size_t sizeToCheck)
{
	return (sizeToCheck + 255) & ~255;
}
