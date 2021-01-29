#pragma once

class CDescriptorHeap;
enum BufferType
{
	CONSTANT_BUFFER,
	TEXTURE_BUFFER
};

class CUploadBuffer
{
public:
	CUploadBuffer(const size_t bufferSize, CDescriptorHeap* descHeap,
                             ID3D12Device2* device,
							const CD3DX12_RESOURCE_DESC& bufferDesc);
	
	bool SubmitData(const void* bData, const size_t dataSize,
                               ID3D12GraphicsCommandList2* commandList);

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress(){return m_myDefaultHeap->GetGPUVirtualAddress();}

private:
	std::shared_ptr<CDescriptorHeap> m_myDescHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_myDefaultHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_myUploadHeap;
	BYTE* m_mappedData;
	size_t m_bufferSize;
};
