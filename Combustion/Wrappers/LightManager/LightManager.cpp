#include <stdafx.h>
#include "LightManager.h"

LightManager::LightManager(int rootIndex) :
	m_rootIndex(rootIndex),
	m_dirty(true)
{
}

LightManager::~LightManager()
{
}

void LightManager::AddLight(Light light)
{
	m_lights.push_back(light);
	m_dirty = true;
}

void LightManager::AddLights(std::vector<Light> lights)
{
	for (int i = 0; i < lights.size(); ++i)
	{
		m_lights.push_back(lights[i]);
	}
	m_dirty = true;
}

void LightManager::RemoveLight(Light light)
{
	for (int i = 0; i < m_lights.size(); ++i)
	{
		if (&m_lights.at(i) == &light)
		{
			m_lights.erase(m_lights.begin() + i);
		}
	}
	m_dirty = true;
}

void LightManager::RemoveAtIndex(int index)
{
	m_lights.erase(m_lights.begin() + index);
	m_dirty = true;
}

bool LightManager::InitializeLightBuffer(CD3DX12_ROOT_PARAMETER1 rootParameters[],
                                         Microsoft::WRL::ComPtr<ID3D12Device2> device,
                                         CDescriptorHeap* descHeap)
{
	m_bufferResource = std::make_unique<CUploadBuffer>(sizeof(Light) * MAX_LIGHTS, descHeap, device.Get(), CD3DX12_RESOURCE_DESC::Buffer(sizeof(Light) * MAX_LIGHTS));


	//m_lightsDefaultHeap->Map(0, nullptr, nullptr);
	//memcpy(m_mappedData,  m_lights.data(), sizeof(Light) * m_lights.size());
	//m_lightsDefaultHeap->Map(0, nullptr, reinterpret_cast<void**>(m_mappedData));
	m_dirty = true;
	return true;
}

bool LightManager::SubmitLightBuffer(ID3D12GraphicsCommandList2* commandList)
{
	if (m_dirty)
	{
		if(m_bufferResource.get() != nullptr)
		{
			m_bufferResource->SubmitData(m_lights.data(), sizeof(Light) * m_lights.size(), commandList);
		}
		else
		{
			throw EXCEPTION_INVALID_HANDLE;
		}
	}
	return true;
}
