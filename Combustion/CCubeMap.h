#pragma once
class CCubeMap : protected ShaderResource
{
public:
	CCubeMap(const wchar_t* dirAddress, ID3D12Device2* device, CDescriptorHeap* descHeap);
	~CCubeMap();

	bool SubmitTexture(DirectX::ScratchImage* image, ID3D12GraphicsCommandList2* commandList) override;
	bool SubmitResource(const void* bData, const size_t dataSize, ID3D12GraphicsCommandList2* commandList) override;
public:
	std::unique_ptr<std::vector<DirectX::ScratchImage>> m_cubeMap;
};

