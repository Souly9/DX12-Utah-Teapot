#pragma once
#include <stdafx.h>

/**
 * Window class to manage a single window and showing the rendered frames
 */
class Window
{
public:
	// Amount of swap chain back buffers
	static const uint8_t BufferCount = 3;

	/**
	 * Return a handle to the current window
	 */
	HWND GetWindow() const;

	void Destroy();

	const std::wstring& GetWindowName() const;

	int GetClientWidth() const;
	int GetClientHeight() const;

	/**
	 * Show or hide the window!
	 */
	void Show();
	void Hide();

	/**
	 * Return the current back buffer index
	 */
	UINT GetCurrentBackBufferIndex() const;

	/**
	 * Present the current back buffer
	 * Returns the current back buffer index
	 */
	UINT Present();

	/**
	 * Returns the current render target view for the back buffer
	 */
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView() const;

	/**
	 * Returns the current back buffer resource of the current back buffer
	 */
	Microsoft::WRL::ComPtr<ID3D12Resource> GetCurrentBackBuffer() const;
	
	 // Register a Program with this window. This allows
    // the window to callback functions in the Program class.
    void RegisterCallbacks( std::shared_ptr<Program> pProgram);

	void OnRender();
	void OnUpdate();
	void OnKeyPressed(WPARAM uint);
	void OnKeyReleased(WPARAM uint);
	void SetFullscreen(bool cond);
	void ToggleFullscreen();
	void OnMouseWheel(float zDelta);
	void OnMouseButtonPressed(int x, int y);
	void OnMouseButtonReleased(int x, int y);
	void OnMouseMoved(bool l_button, int x, int y);
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() {return m_d3d12RTVDescriptorHeap;}
protected:

	// callback of the window procedure
	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	// Window creation is handled by WindowManager
	friend class WindowManager;
	// The DirectXTemplate class needs to register with a window
	friend class Program;

	Window() = delete;
	Window(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync );
    virtual ~Window();

   
	
    // Create the ch chain
    Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain();

    // Update the render target views for the swap chain back buffers.
    void UpdateRenderTargetViews();

private:
    // Windows should not be copied.
    Window(const Window& copy) = delete;

	void TickClock(HighResClock* clock);

    HWND m_hWnd;

    std::wstring m_WindowName;
    
    int m_ClientWidth;
    int m_ClientHeight;
	bool m_fullscreen;

    HighResClock m_updateClock;
    HighResClock m_renderClock;
    uint64_t m_FrameCounter;

    std::weak_ptr<Program> m_pProgram;

    Microsoft::WRL::ComPtr<IDXGISwapChain4> m_dxgiSwapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_d3d12RTVDescriptorHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_d3d12BackBuffers[BufferCount];

    UINT m_RTVDescriptorSize;
    UINT m_CurrentBackBufferIndex;

    RECT m_WindowRect;

	int m_prevMouseX, m_prevMouseY;
};
