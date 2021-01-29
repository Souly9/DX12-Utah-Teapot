#pragma once
#include <DirectXTex.h>

class ShaderResource
{
public:
	~ShaderResource();
	virtual bool SubmitTexture(DirectX::ScratchImage* image,
                               ID3D12GraphicsCommandList2* commandList);
	virtual bool SubmitResource(const void* bData, const size_t dataSize, ID3D12GraphicsCommandList2* commandList);
	ShaderResource(ShaderResource& other);
	ShaderResource();
	
	ShaderResource(const size_t bufferSize, CDescriptorHeap* descHeap,
                             ID3D12Device2* device,
							const CD3DX12_RESOURCE_DESC& bufferDesc,
							BufferType bufferType, DirectX::ScratchImage* image);
protected:
	
	
	
	static constexpr size_t ValidateBufferSize(size_t sizeToCheck);
	
	size_t m_bufferSize;

	std::shared_ptr<CDescriptorHeap> m_myDescHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_myDefaultHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_myUploadHeap;
};

