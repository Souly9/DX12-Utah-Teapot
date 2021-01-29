#include <stdafx.h>
#include "TextureReader.h"

std::unique_ptr<TextureReader> TextureReaderSingleton::m_textureReader;

TextureReader::TextureReader()
{
	ThrowIfFailed(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
}

TextureReader::~TextureReader()
{
}

void TextureReader::ReadFromFile(const wchar_t* dirAddress, DirectX::ScratchImage& image,  DirectX::TexMetadata* metadata, DirectX::WIC_FLAGS flags)
{
	ThrowIfFailed(DirectX::LoadFromWICFile(dirAddress, flags, nullptr, image));
}
