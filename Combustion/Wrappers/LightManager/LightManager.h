#pragma once
#include "CUploadBuffer.h"

class ShaderResource;

class LightManager
{
public:
	LightManager(int rootIndex);
	~LightManager();

	void AddLight(Light light);
	void AddLights(std::vector<Light> lights);

	void RemoveLight(Light light);
	void RemoveAtIndex(int index);

	bool InitializeLightBuffer(CD3DX12_ROOT_PARAMETER1 rootParameters[], Microsoft::WRL::ComPtr<ID3D12Device2> device,  CDescriptorHeap* descHeap);
	bool SubmitLightBuffer(ID3D12GraphicsCommandList2* commandList);

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress(){return m_bufferResource->GetGPUAddress();}
	
private:
	LightManager();
	std::vector<Light> m_lights;

	// Index used to refer to the constant buffer assigned to this manager
	int m_rootIndex = 1;
	std::unique_ptr<CUploadBuffer> m_bufferResource;
	
	bool m_dirty;
};


