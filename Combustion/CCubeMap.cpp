#include <stdafx.h>
#include "CCubeMap.h"

CCubeMap::CCubeMap(const wchar_t* dirAddress, ID3D12Device2* device, CDescriptorHeap* descHeap)
{
	m_cubeMap = std::make_unique<std::vector<DirectX::ScratchImage>>();

	std::wstring fileAddress(dirAddress);
	std::vector<const wchar_t*> fileNames;
	fileNames.push_back(L"/negx.jpg");
	fileNames.push_back(L"/negy.jpg");
	fileNames.push_back(L"/negz.jpg");
	fileNames.push_back(L"/posx.jpg");
	fileNames.push_back(L"/posy.jpg");
	fileNames.push_back(L"/posz.jpg");
	for(int i = 0; i < 6; ++i)
	{
		auto image = std::make_unique<DirectX::ScratchImage>();
		size_t size = sizeof(&fileNames.at(i));
		fileAddress = fileAddress.append(fileNames.at(i));
		TextureReaderSingleton::GetReader()->ReadFromFile(
			fileAddress.c_str(), 
			*image
		);
		m_cubeMap->push_back(std::move(*image));
		fileAddress = fileAddress.substr(0, fileAddress.length() - size -1);
	}

	
	DirectX::ScratchImage img;
	const DirectX::Image imgs[] =
	{
		m_cubeMap->at(0).GetImages()[0],
		m_cubeMap->at(1).GetImages()[0],
		m_cubeMap->at(2).GetImages()[0],
		m_cubeMap->at(3).GetImages()[0],
		m_cubeMap->at(4).GetImages()[0],
		m_cubeMap->at(5).GetImages()[0]
	};

	img.InitializeCubeFromImages(
		imgs,
		6);

	size_t size = ValidateBufferSize(img.GetPixelsSize());
	m_bufferSize = size;
	const auto typeDef =	CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const auto desc =	CD3DX12_RESOURCE_DESC::Tex2D(
			img.GetMetadata().format, 
			img.GetMetadata().width, 
			img.GetMetadata().height
		);
	
	ThrowIfFailed(device->CreateCommittedResource(
		&typeDef,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_myDefaultHeap)));

	const auto& typeUp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const auto& bDesc = CD3DX12_RESOURCE_DESC::Buffer(m_bufferSize);
	ThrowIfFailed(device->CreateCommittedResource(
		&typeUp,
		D3D12_HEAP_FLAG_NONE,
		&bDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_myUploadHeap)));

	
	D3D12_SHADER_RESOURCE_VIEW_DESC cubeView = {};
	cubeView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	cubeView.Format = img.GetMetadata().format;
	cubeView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	cubeView.TextureCube.MipLevels = 1;
	cubeView.TextureCube.MostDetailedMip = 0;
	cubeView.TextureCube.ResourceMinLODClamp = 0;
	
	device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle0( descHeap->GetCPUHandle(), 1, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	device->CreateShaderResourceView(m_myDefaultHeap.Get(), &cubeView, srvHandle0);
}

CCubeMap::~CCubeMap()
{
}

bool CCubeMap::SubmitTexture(DirectX::ScratchImage* image, ID3D12GraphicsCommandList2* commandList)
{
	D3D12_SUBRESOURCE_DATA bufferData = {};
		bufferData.pData = image->GetPixels();
		bufferData.RowPitch = image->GetImages()->rowPitch;
		bufferData.SlicePitch = image->GetImages()->slicePitch;

		ThrowIfFailed(UpdateSubresources(commandList, m_myDefaultHeap.Get(), 
			m_myUploadHeap.Get(), 0, 0, 1, 
			&bufferData));

	const auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_myDefaultHeap.Get(),
                                                                           D3D12_RESOURCE_STATE_COPY_DEST,
                                                                           D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		commandList->ResourceBarrier(1,
                                     &transition);
		return true;
}

bool CCubeMap::SubmitResource(const void* bData, const size_t dataSize, ID3D12GraphicsCommandList2* commandList)
{
	return true;
}

