
#include <stdafx.h>
#include "TextureResource.h"

CTextureResource::CTextureResource(const wchar_t* address, 
					CDescriptorHeap* descHeap,
                             ID3D12Device2* device)
{
	m_image = std::make_unique<DirectX::ScratchImage>();
	TextureReaderSingleton::GetReader()->ReadFromFile(address, *m_image);
	m_buffer = std::make_unique<ShaderResource>(m_image->GetPixelsSize(),
		descHeap,
		device, 
		CD3DX12_RESOURCE_DESC::Tex2D(m_image->GetMetadata().format, m_image->GetMetadata().width, m_image->GetMetadata().height),
		BufferType::TEXTURE_BUFFER,
		m_image.get());
	
}

CTextureResource::~CTextureResource()
{
}

bool CTextureResource::LoadTexture(DirectX::ScratchImage* image, ID3D12GraphicsCommandList2* commandList)
{
	return m_buffer->SubmitTexture(image, commandList);
}
