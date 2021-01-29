#pragma once
#include <TextureReader.h>
/**
 * Wrapper class for one texture
 * Offers the ability to load and bind the texture to a DescriptorHeap
 */
class CTextureResource
{
public:
	CTextureResource(const wchar_t* address, 
					CDescriptorHeap* descHeap,
                             ID3D12Device2* device);
	~CTextureResource();

	bool LoadTexture(DirectX::ScratchImage* image, ID3D12GraphicsCommandList2* commandList);
	std::unique_ptr<DirectX::ScratchImage> m_image;
private:
	CTextureResource();
	std::unique_ptr<ShaderResource> m_buffer;
	const char* m_fileName;
};
