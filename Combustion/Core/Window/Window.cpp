#include "stdafx.h"
class Renderer;


Window::Window(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync )
    : m_hWnd(hWnd)
    , m_WindowName(windowName)
    , m_ClientWidth(clientWidth)
    , m_ClientHeight(clientHeight)
    , m_FrameCounter(0)
{
    WindowManager& app = WindowManager::Get();


    m_dxgiSwapChain = CreateSwapChain();
    m_d3d12RTVDescriptorHeap = app.CreateDescriptorHeap(BufferCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_RTVDescriptorSize = app.GetDescriptHandlerIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	m_updateClock.Reset();
	m_renderClock.Reset();
    UpdateRenderTargetViews();
}

Window::~Window()
{
	assert(!m_hWnd && "Call WindowManager::DestroyWindow before destruction!");
}

HWND Window::GetWindow() const
{
    return m_hWnd;
}



const std::wstring& Window::GetWindowName() const
{
    return m_WindowName;
}

void Window::Show()
{
    ::ShowWindow(m_hWnd, SW_SHOW);
}

/**
* Hide the window.
*/
void Window::Hide()
{
    ::ShowWindow(m_hWnd, SW_HIDE);
}

void Window::Destroy()
{
    if (auto pProgram = m_pProgram.lock())
    {
        // Notify the registered game that the window is being destroyed.
        pProgram->OnWindowDestroy();
    }
    if (m_hWnd)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
    }
}

int Window::GetClientWidth() const
{
    return m_ClientWidth;
}

int Window::GetClientHeight() const
{
    return m_ClientHeight;
}

void Window::RegisterCallbacks(std::shared_ptr<Program> pProgram)
{
    m_pProgram = pProgram;

	return;
}

void Window::OnUpdate()
{
    TickClock(&m_updateClock);

    if (auto pProgram = m_pProgram.lock())
    {
        m_FrameCounter++;
    	
        pProgram->OnUpdate(m_updateClock.GetDeltaSeconds(), m_updateClock.GetTotalSeconds());
    }
}

void Window::OnKeyPressed(WPARAM uint)
{
	if (auto pProgram = m_pProgram.lock())
    {
        pProgram->OnKeyPressed(uint);
    }
}

void Window::OnKeyReleased(WPARAM uint)
{
	if (auto pProgram = m_pProgram.lock())
    {
       pProgram->OnKeyReleased(uint);
    }
}

void Window::SetFullscreen(bool cond)
{
	  if (m_fullscreen != cond)
    {
        m_fullscreen = cond;

        if (m_fullscreen) // Switching to fullscreen.
        {
            // Store the current window dimensions so they can be restored 
            // when switching out of fullscreen state.
            ::GetWindowRect(m_hWnd, &m_WindowRect);

            // Set the window style to a borderless window so the client area fills
            // the entire screen.
            UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

            ::SetWindowLongW(m_hWnd, GWL_STYLE, windowStyle);

            // Query the name of the nearest display device for the window.
            // This is required to set the fullscreen dimensions of the window
            // when using a multi-monitor setup.
            HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFOEX monitorInfo = {};
            monitorInfo.cbSize = sizeof(MONITORINFOEX);
            ::GetMonitorInfo(hMonitor, &monitorInfo);

            ::SetWindowPos(m_hWnd, HWND_TOPMOST,
                monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_FRAMECHANGED | SWP_NOACTIVATE);

            ::ShowWindow(m_hWnd, SW_MAXIMIZE);
        }
        else
        {
            // Restore all the window decorators.
            ::SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

            ::SetWindowPos(m_hWnd, HWND_NOTOPMOST,
                m_WindowRect.left,
                m_WindowRect.top,
                m_WindowRect.right - m_WindowRect.left,
                m_WindowRect.bottom - m_WindowRect.top,
                SWP_FRAMECHANGED | SWP_NOACTIVATE);

            ::ShowWindow(m_hWnd, SW_NORMAL);
        }
    }
}

void Window::ToggleFullscreen()
{
	SetFullscreen(!m_fullscreen);
}

void Window::OnMouseWheel(float zDelta)
{
	 if (auto pProgram = m_pProgram.lock())
    {
        pProgram->OnMouseWheel(zDelta);
    }
}

void Window::OnMouseButtonPressed(int x, int y)
{
	 if (auto pProgram = m_pProgram.lock())
    {
        pProgram->OnMouseButtonPressed(x, y);
    }
}

void Window::OnMouseButtonReleased(int x, int y)
{
	if (auto pProgram = m_pProgram.lock())
    {
        pProgram->OnMouseButtonReleased(x, y);
    }
}

void Window::OnMouseMoved(bool l_button, int x, int y)
{
	if (l_button)
	{
		if (auto pProgram = m_pProgram.lock())
		{
			pProgram->OnMouseMoved(x, y);
		}
	}
}

void Window::OnRender()
{
    TickClock(&m_renderClock);

    if (auto pProgram = m_pProgram.lock())
    {
        pProgram->OnRender(m_renderClock.GetDeltaSeconds(), m_renderClock.GetTotalSeconds());
    }
}

Microsoft::WRL::ComPtr<IDXGISwapChain4> Window::CreateSwapChain()
{
    WindowManager& app = WindowManager::Get();

    Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain4;
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory4;
      UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = 2560;
	swapChainDesc.Height = 1440;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = {1, 0};
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = BufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// It is recommended to always allow tearing if tearing support is available.
	swapChainDesc.Flags = 0;
    ID3D12CommandQueue* pCommandQueue = app.GetCommandQueue()->GetD3D12CommandQueue().Get();

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
    ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(
        pCommandQueue,
        m_hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1));

    // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
    // will be handled manually.
    ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain1.As(&dxgiSwapChain4));

    m_CurrentBackBufferIndex = dxgiSwapChain4->GetCurrentBackBufferIndex();

    return dxgiSwapChain4;
}

// Update the render target views for the swapchain back buffers.
void Window::UpdateRenderTargetViews()
{
    auto device = WindowManager::Get().GetDevice();

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_d3d12RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    for (int i = 0; i < BufferCount; ++i)
    {
	    Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
        ThrowIfFailed(m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

        device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

        m_d3d12BackBuffers[i] = backBuffer;

        rtvHandle.Offset(m_RTVDescriptorSize);
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE Window::GetCurrentRenderTargetView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_d3d12RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        m_CurrentBackBufferIndex, m_RTVDescriptorSize);
}

Microsoft::WRL::ComPtr<ID3D12Resource> Window::GetCurrentBackBuffer() const
{
    return m_d3d12BackBuffers[m_CurrentBackBufferIndex];
}

UINT Window::GetCurrentBackBufferIndex() const
{
    return m_CurrentBackBufferIndex;
}

UINT Window::Present()
{
    UINT syncInterval = 1;
    UINT presentFlags = 0;
    ThrowIfFailed(m_dxgiSwapChain->Present(syncInterval, presentFlags));
    m_CurrentBackBufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

    return m_CurrentBackBufferIndex;
}

void Window::TickClock(HighResClock* clock)
{
	clock->Tick();
}

