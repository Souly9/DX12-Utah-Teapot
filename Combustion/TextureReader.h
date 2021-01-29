#pragma once
#include <DirectXTex.h>
class TextureReader
{
public:
	TextureReader();
	~TextureReader();
	
	void ReadFromFile(const wchar_t* dirAddress, DirectX::ScratchImage& image,  DirectX::TexMetadata* metadata = nullptr, DirectX::WIC_FLAGS flags = DirectX::WIC_FLAGS_NONE);

private:
	
};



static class TextureReaderSingleton
{
public:
	static std::unique_ptr<TextureReader> GetReader()
	{
		if(!m_textureReader)
		{
			m_textureReader = std::make_unique<TextureReader>();
		}
		return std::move(m_textureReader);
	}
private:
	static std::unique_ptr<TextureReader> m_textureReader;
};

