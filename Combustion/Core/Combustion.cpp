#include <stdafx.h>

#include "Combustion.h"

struct Matrices
{
	DirectX::XMMATRIX Model;
	DirectX::XMMATRIX MVP;
	DirectX::XMFLOAT3 CameraPos;
};

Matrices matrices;

Combustion::Combustion(const std::wstring& name, int width, int height, bool vSync)
	: super(name, width, height, vSync)
	  , m_viewport(CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)))
	  , m_scissorRect(CD3DX12_RECT(0, 0,LONG_MAX, LONG_MAX))
	  , m_FoV(90)
	  , m_contentLoaded(false)
	  , m_lightManager(0)
{
	m_light = PointLight(DirectX::XMFLOAT4(0, 0, -30, 0), DirectX::XMFLOAT4(0.2f, 0.5f, 0.1f, 1));
	m_camera = std::make_unique<Camera>(m_FoV, 0.1f, 100.0f, GetWidth() / static_cast<float>(GetHeight()),
	                  DirectX::XMVectorSet(0, 0, m_distance, 0), DirectX::XMVectorSet(0, 0, 0, 1));

	m_lightManager.AddLight(m_light);
	m_myArcBall = std::make_unique<ArcBall>();
	m_myArcBall->UpdatePos(
	DirectX::XMFLOAT2(m_width / 2.0f, m_height / 2.0f),
		0.5f * static_cast<float>(sqrt(static_cast<float>(m_width * m_width + m_height * m_height))));
	
	// Update the view matrix
		const DirectX::XMVECTOR eyePosition = DirectX::XMVectorSet(0, 0, m_distance, 1);
		const DirectX::XMVECTOR focusPosition = DirectX::XMVectorSet(0, 0, 10, 1);
		const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
		m_camera->LookAt(eyePosition, focusPosition, upDirection);
}

void Combustion::OnUpdate(float deltaTime, float totalTime)
{
	static uint64_t frameCount = 0;
	static double elapsedTime = 0.0;

	super::OnUpdate(deltaTime, totalTime);

	elapsedTime += deltaTime;
	frameCount++;
	char buffer[512];

	if (elapsedTime > 1.0)
	{
		double fps = frameCount / elapsedTime;


		sprintf_s(buffer, "FPS: %f\n", fps);
		OutputDebugStringA(buffer);

		frameCount = 0;
		elapsedTime = 0.0;
	}

	// Update our matrices!
	// Update the model matrix
	float angle = static_cast<float>(m_pitch * 0.1);

	m_light.SetPosition(DirectX::XMFLOAT4(m_light.GetPosition().x, m_light.GetPosition().y + 1, m_light.GetPosition().z,
	                                      1));
	m_lightManager.RemoveAtIndex(0);
	m_lightManager.AddLight(m_light);
	
	 m_myArcBall->GetRotationMatrix(m_modelMatrix);

		
	 	m_cameraInt = true;
	
}

void Combustion::OnRender(float deltaTime, float totalTime)
{
	super::OnRender(deltaTime, totalTime);
	
	auto commandQueue = WindowManager::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	auto commandList = commandQueue->GetCommandList();
	
	UINT currentBackBufferIndex = m_pWindow->GetCurrentBackBufferIndex();
	auto backBuffer = m_pWindow->GetCurrentBackBuffer();
	auto rtv = m_pWindow->GetCurrentRenderTargetView();
	auto dsv = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();

	// Clear the render targets
	{
		TransitionResource(commandList, backBuffer,
		                   D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		FLOAT clearColor[] = {0.4f, 0.6f, 0.9f, 1.0f};

		ClearRTV(commandList, rtv, clearColor);
		ClearDepth(commandList, dsv);
	}
	commandList->SetPipelineState(m_pipelineState.Get());
	ID3D12DescriptorHeap* descriptorHeaps[] = {m_shaderBufferHeap->GetHeap().Get()};
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	commandList->SetGraphicsRootSignature(m_rootSignature.Get());
	// Instruct the Input Assembler
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	commandList->IASetIndexBuffer(&m_indexBufferView);

	// Set the viewport and scissor rectangle
	commandList->RSSetViewports(1, &m_viewport);
	commandList->RSSetScissorRects(1, &m_scissorRect);

	// Render target must be bound to the output merger
	commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

	// Update the MVP matrix
	m_camera->UpdateMatrices();
	DirectX::XMMATRIX mvpMatrix = XMMatrixMultiply(m_modelMatrix, m_camera->GetViewProjectionMatrix());
	matrices.MVP = mvpMatrix;
	matrices.Model = m_modelMatrix;
	matrices.CameraPos = m_camera->GetPosition();


	float angle = static_cast<float>(m_pitch * 0.1);
	const DirectX::XMVECTOR rotationAxis = DirectX::XMVectorSet(0, 1, 0, 0);
	m_modelMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, DirectX::XMConvertToRadians(angle));
	// Update the view matrix
	commandList->SetGraphicsRoot32BitConstants(0, sizeof(Matrices) / 4, &matrices, 0);
	
	commandList->SetGraphicsRootConstantBufferView(1, m_lightManager.GetGPUAddress());
	commandList->SetGraphicsRootDescriptorTable(2, m_shaderBufferHeap->GetGPUHandle());
	//CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle0( m_shaderBufferHeap->GetGPUHandle(), 1, WindowManager::Get().GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	//commandList->SetGraphicsRootDescriptorTable(2, srvHandle0);
	
	commandList->DrawIndexedInstanced(m_teapot.m_numPrimitives, 1, 0, 0, 0);

	// Present
	{
		TransitionResource(commandList, backBuffer,
		                   D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		m_fenceValues[currentBackBufferIndex] = commandQueue->ExecuteCommandList(commandList);
		
		currentBackBufferIndex = m_pWindow->Present();

		commandQueue->WaitForFenceValue(m_fenceValues[currentBackBufferIndex]);
	}
}

void Combustion::UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
                                      ID3D12Resource** pDestinationResource, ID3D12Resource** pIntermediateResource,
                                      size_t numElements, size_t elementSize, const void* bufferData,
                                      D3D12_RESOURCE_FLAGS flags)
{
	auto device = WindowManager::Get().GetDevice();

	size_t bufferSize = numElements * elementSize;
	const auto type = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const auto buffer = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);
	ThrowIfFailed(device->CreateCommittedResource(
		&type,
		D3D12_HEAP_FLAG_NONE,
		&buffer,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(pDestinationResource)));

	if (bufferData)
	{
		const auto typeUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const auto bufferUpload = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
		ThrowIfFailed(device->CreateCommittedResource(
			&typeUpload,
			D3D12_HEAP_FLAG_NONE,
			&bufferUpload,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(pIntermediateResource)));

		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = bufferData;
		subResourceData.RowPitch = bufferSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		ThrowIfFailed(UpdateSubresources(commandList.Get(),
		                                 *pDestinationResource, *pIntermediateResource,
		                                 0, 0, 1, &subResourceData));
	}
}

void Combustion::OnKeyReleased(WPARAM uint)
{
}

void Combustion::OnMouseWheel(float zDelta)
{
	if (m_distance <= -10)
		m_distance -= (zDelta) * -10;
}

void Combustion::TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
                                    Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES beforeState,
                                    D3D12_RESOURCE_STATES afterState)
{
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		resource.Get(),
		beforeState, afterState);

	commandList->ResourceBarrier(1, &barrier);
}

// Clear a render target.
void Combustion::ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
                          D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor)
{
	commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

void Combustion::ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
                            D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth)
{
	commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

bool Combustion::LoadContent()
{
	// Load the famous Utah teapot model
	ObjReader reader;
	m_teapot = reader.readOBJ("D:/Programme/FH/Cuda/RayTracerDX12/input/utah-teapot.obj");

	// Load a Command list for our vertex buffers
	auto device = WindowManager::Get().GetDevice();
	auto commandQueue = WindowManager::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
	auto commandList = commandQueue->GetCommandList();

	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateVertexBuffer;
	UpdateBufferResource(commandList,
	                     &m_vertexBuffer, &intermediateVertexBuffer,
	                     m_teapot.m_vertices.size(), sizeof(Vertex), m_teapot.m_vertices.data());

	// Set the Vertex buffer view to address the buffer
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);
	m_vertexBufferView.SizeInBytes = m_teapot.m_vertices.size() * sizeof(Vertex);

	// Upload index buffer data.
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateIndexBuffer;
	UpdateBufferResource(commandList,
	                     &m_indexBuffer, &intermediateIndexBuffer,
	                     m_teapot.m_numPrimitives, sizeof(unsigned int), m_teapot.m_indices.data());

	// Create a view for the index buffer
	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = m_teapot.m_indices.size() * sizeof(unsigned int);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap)));

	// Load Vertex Shader
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(L"VertexShader.cso", &vertexShaderBlob));

	// Load Pixel Shader
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
	ThrowIfFailed(D3DReadFileToBlob(L"PixelShader.cso", &pixelShaderBlob));

	
	// Describe the data in our buffer
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},

		{
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// Create a root signature to point towards descriptor ranges
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	// Set the flags to define the input layout and deny unnecessary access to certain pipeline stages
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	CD3DX12_DESCRIPTOR_RANGE1 shaderDescriptorRange[2];
	shaderDescriptorRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0);
	shaderDescriptorRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

	
	// 32-bit constant root parameters that are used by the shaders
	CD3DX12_ROOT_PARAMETER1 rootParameters[3];
	rootParameters[0].InitAsConstants(sizeof(Matrices) / 4, 0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[1].InitAsConstantBufferView(rootParameters[1], 1, 0);
	rootParameters[2].InitAsDescriptorTable(1, &shaderDescriptorRange[1], D3D12_SHADER_VISIBILITY_PIXEL);
	
	m_shaderBufferHeap = std::make_unique<CDescriptorHeap>(100, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
	                                                 D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
	                                                 device.Get());
	
	m_lightManager.InitializeLightBuffer(rootParameters, device, m_shaderBufferHeap.get());
	
	m_texture = std::make_unique<CTextureResource>(L"Rock/Rock035_2K_Color.jpg", m_shaderBufferHeap.get(), device.Get());
	
	auto commandQ2 = WindowManager::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	auto commandL2 = commandQ2->GetCommandList();
	
	m_texture->LoadTexture(m_texture->m_image.get(), commandL2.Get());
	
	m_lightManager.SubmitLightBuffer(commandL2.Get());

	auto val = commandQ2->ExecuteCommandList(commandL2);
	commandQ2->WaitForFenceValue(val);
	
    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.MipLODBias = 0;
    sampler.MaxAnisotropy = 16;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    sampler.MinLOD = 0;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	
	// Root signature is created
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

	// Serialize the root signature
	Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc,
	                                                    featureData.HighestVersion, &rootSignatureBlob, &errorBlob));
	// Create the root signature
	ThrowIfFailed(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
	                                          rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	// The PSO
	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
	} pipelineStateStream;


	// Let's fill in the previously defined attributes of the Pipeline State Stream
	pipelineStateStream.pRootSignature = m_rootSignature.Get();
	pipelineStateStream.InputLayout = {inputLayout, _countof(inputLayout)};
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
	pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.RTVFormats = rtvFormats;
	CD3DX12_RASTERIZER_DESC rsDesc(D3D12_DEFAULT);
	rsDesc.CullMode = D3D12_CULL_MODE_NONE;
	pipelineStateStream.Rasterizer = rsDesc;

	// Now we can create the Pipeline State Stream
	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
		sizeof(PipelineStateStream), &pipelineStateStream
	};

	ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pipelineState)));

	
	// Now we can execute the commandlist!
	auto fenceValue = commandQueue->ExecuteCommandList(commandList);
	commandQueue->WaitForFenceValue(fenceValue);
	
	m_contentLoaded = true;

	// Create the depth buffer
	ResizeDepthBuffer(GetWidth(), GetHeight());

	return true;
}

void Combustion::UnloadContent()
{
}

void Combustion::ResizeDepthBuffer(int width, int height)
{
	if (m_contentLoaded)
	{
		// Flush any commands that may access the depth buffer
		WindowManager::Get().Flush();

		width = std::max(1, width);
		height = std::max(1, height);

		auto device = WindowManager::Get().GetDevice();

		// Resize screen dependent resources
		// Creates a depth buffer
		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = {1.0f, 0};

		const auto type = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		const auto stencil = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height,
			                              1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		ThrowIfFailed(device->CreateCommittedResource(
			&type,
			D3D12_HEAP_FLAG_NONE,
			&stencil,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			IID_PPV_ARGS(&m_depthBuffer)));

		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
		dsv.Format = DXGI_FORMAT_D32_FLOAT;
		dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv.Texture2D.MipSlice = 0;
		dsv.Flags = D3D12_DSV_FLAG_NONE;

		device->CreateDepthStencilView(m_depthBuffer.Get(), &dsv,
		                               m_DSVHeap->GetCPUDescriptorHandleForHeapStart());
	}
}

void Combustion::OnKeyPressed(WPARAM uint)
{
	if (uint == VK_ESCAPE)
	{
		WindowManager::Get().Quit(0);
	}
	if (uint == VK_F11 || (uint == VK_RETURN && uint == VK_MENU))
	{
		m_pWindow->ToggleFullscreen();
	}
}

void Program::OnMouseMoved(int x, int y)
{
}

void Combustion::OnMouseButtonPressed(int x, int y)
{
	m_myArcBall->SetMousePos(DirectX::XMFLOAT2(x, y));
	m_myArcBall->StartDragging();
}
void Combustion::OnMouseButtonReleased(int x, int y)
{
	m_myArcBall->SetMousePos(DirectX::XMFLOAT2(x, y));
	m_myArcBall->StopDragging();
}

void Combustion::OnMouseMoved(int x, int y)
{
	m_myArcBall->SetMousePos(DirectX::XMFLOAT2(x, y));
	
}
