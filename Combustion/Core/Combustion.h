#pragma once



class Combustion : public Program
{
public:
	using super = Program;

	Combustion(const std::wstring& name, int width, int height, bool vSync);

	/**
	 * Load all required content
	 */
	virtual bool LoadContent() override;

	/**
	 * Free up all the resources we initially loaded
	 */
	virtual void UnloadContent() override;

	virtual void OnUpdate(float deltaTime, float totalTime) override;
	
	virtual void OnRender(float deltaTime, float totalTime) override;
	virtual void OnKeyPressed(WPARAM uint) override;
	virtual void OnKeyReleased(WPARAM uint) override;
	virtual void OnMouseWheel(float zDelta) override;
	void OnMouseButtonPressed(int x, int y) override;
	virtual void OnMouseMoved(int x, int y) override;
	virtual void OnMouseButtonReleased(int x, int y) override;
protected:
	

private:
	// Transition a resource
    void TransitionResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
        Microsoft::WRL::ComPtr<ID3D12Resource> resource,
        D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);
 
    // Clear a render target view.
    void ClearRTV(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
        D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT* clearColor);
 
    // Clear the depth of a depth-stencil view.
    void ClearDepth(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
        D3D12_CPU_DESCRIPTOR_HANDLE dsv, FLOAT depth = 1.0f );
 
    // Create a GPU buffer.
    void UpdateBufferResource(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
        ID3D12Resource** pDestinationResource, ID3D12Resource** pIntermediateResource,
        size_t numElements, size_t elementSize, const void* bufferData, 
        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE );
 
    // Resize the depth buffer to match the size of the client area.
    void ResizeDepthBuffer(int width, int height);

	uint64_t m_fenceValues[Window::BufferCount] = {};

	// Buffers needed for the scene
	// Vertex Buffer
	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	// Index Buffer
	Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

	// Depth Buffer
	Microsoft::WRL::ComPtr<ID3D12Resource> m_depthBuffer;
	// Descriptor heap for depth buffer
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
	std::unique_ptr<CDescriptorHeap> m_shaderBufferHeap;
	std::unique_ptr<CDescriptorHeap> m_samplerHeap;
	// Rendering pipeline variables
	// Root signature
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

	// Pipeline state object
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeaps;
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

	float m_FoV;

	DirectX::XMMATRIX m_modelMatrix;
	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_projectionMatrix;

	bool m_contentLoaded;

	// Important for rendering
	Mesh3D m_teapot;
	int m_distance = -50;
	std::unique_ptr<Camera> m_camera;
	PointLight m_light;
	LightManager m_lightManager;
	std::unique_ptr<ArcBall> m_myArcBall;
	bool m_cameraInt;
	std::unique_ptr<CTextureResource> m_texture;
	std::unique_ptr<CCubeMap> m_cube;
	DirectX::ScratchImage img;
	std::vector<DirectX::Image> imgs;
};