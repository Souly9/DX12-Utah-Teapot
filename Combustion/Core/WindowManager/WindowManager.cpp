#include <stdafx.h>

constexpr wchar_t WINDOW_CLASS_NAME[] = L"DX12RendererWindowManager";

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

static WindowManager* gs_pSingelton;
static std::map<HWND, std::shared_ptr<Window>> gs_windows;
static std::map<std::wstring, std::shared_ptr<Window>> gs_windowByName;

 int WindowManager::Run(std::shared_ptr<Program> pProgram)
{
	if(!pProgram->Initialize()) return 1;
	if(!pProgram->LoadContent()) return 2;

	
	// Typical Windows loop
    MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	Flush();

	pProgram->UnloadContent();
	pProgram->Destroy();
	
	return 0;
}

// A wrapper struct to allow shared pointers for the window class.
struct MakeWindow : public Window 
{
    MakeWindow(HWND hWnd, const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync)
        : Window(hWnd, windowName, clientWidth, clientHeight, vSync)
    {}
};

WindowManager::WindowManager(HINSTANCE hInst)
	:m_hInstance(hInst)
{
	 SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
	debugInterface->EnableDebugLayer();
#endif

	//create the 
	 WNDCLASSEXW wndClass = { 0 };

    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = &WndProc;
    wndClass.hInstance = m_hInstance;
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(5));
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = WINDOW_CLASS_NAME;
    wndClass.hIconSm = LoadIcon(m_hInstance, MAKEINTRESOURCE(5));

	if(!RegisterClassExW(&wndClass))
	{
		MessageBoxA(NULL, "Unable to register the window class!", "Error", MB_OK|MB_ICONERROR);
	}

	m_dxgiAdapter = GetAdapter();
	// Check for Errors with the Adapter creation
	if(m_dxgiAdapter)
	{
		m_d3d12Device = CreateDevice(m_dxgiAdapter);
		// Check for Errors with the Device creation
		if(m_d3d12Device)
		{
			m_directCommandQueue = std::make_shared<CommandQueue>(m_d3d12Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
			m_computeCommandQueue = std::make_shared<CommandQueue>(m_d3d12Device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
			m_copyCommandQueue = std::make_shared<CommandQueue>(m_d3d12Device, D3D12_COMMAND_LIST_TYPE_COPY);
	
		}
		else
			MessageBoxA(NULL, "D3D12 Device couldn't be created!", "Error", MB_OK);
	
	}
	else
		MessageBoxA(NULL, "DXGIAdapter couldn't be created!", "Error", MB_OK);
}

 void WindowManager::Create(HINSTANCE hInst)
{
	if(!gs_pSingelton)
	{
		gs_pSingelton = new WindowManager(hInst);
	}
}

 WindowManager::~WindowManager()
{
	Flush();
}

 void WindowManager::Destroy()
{
	if (gs_pSingelton)
    {
        assert( gs_windows.empty() && gs_windowByName.empty() && 
            "All windows should be destroyed before destroying the application instance.");

        delete gs_pSingelton;
        gs_pSingelton = nullptr;
    }
}

 WindowManager& WindowManager::Get()
{
	assert(gs_pSingelton);
	return *gs_pSingelton;
}

 Microsoft::WRL::ComPtr<IDXGIAdapter4> WindowManager::GetAdapter()
{
	// DXGIFactory is needed before an adapter can be found
	Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
	UINT createFactoryFlags = 0;
#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
	ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS((&dxgiFactory))));

	Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgiAdapter1;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter4;

	SIZE_T maxVideoMemory = 0;
	// Iterate through all devices to find the DX12 compatible one with the most video memory
	for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND;++i)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
		dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

		if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
			SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
				D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
			dxgiAdapterDesc1.DedicatedVideoMemory > maxVideoMemory)
		{
			maxVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
			ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
		}
	}

	return dxgiAdapter4;

}

 Microsoft::WRL::ComPtr<ID3D12Device2> WindowManager::CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter)
{
	Microsoft::WRL::ComPtr<ID3D12Device2> d3d12Device2;
	ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device2)));
	// Enable debug messages in debug mode.
#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfoQueue;
	if (SUCCEEDED(d3d12Device2.As(&pInfoQueue)))
	{
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

		// Suppress whole categories of messages
		//D3D12_MESSAGE_CATEGORY Categories[] = {};

		// Suppress messages based on their severity level
		D3D12_MESSAGE_SEVERITY Severities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO
		};

		// Suppress individual messages by their ID
		D3D12_MESSAGE_ID DenyIds[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
			// I'm really not sure how to avoid this message.
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
			// This warning occurs when using capture frame while graphics debugging.
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
			// This warning occurs when using capture frame while graphics debugging.
		};

		D3D12_INFO_QUEUE_FILTER NewFilter = {};
		NewFilter.DenyList.NumSeverities = _countof(Severities);
		NewFilter.DenyList.pSeverityList = Severities;
		NewFilter.DenyList.NumIDs = _countof(DenyIds);
		NewFilter.DenyList.pIDList = DenyIds;

		ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
	}
#endif

	return d3d12Device2;
}

 std::shared_ptr<Window> WindowManager::CreateRenderWindow(const std::wstring& windowName, int width, int height, bool vSync)
{
	// Do we have this window?
	auto windowIter = gs_windowByName.find(windowName);
    if (windowIter != gs_windowByName.end())
    {
        return windowIter->second;
    }

	RECT windowRect = {0,0, width, height };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hWnd = CreateWindowW(WINDOW_CLASS_NAME, windowName.c_str(),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr, nullptr, m_hInstance, nullptr);
	 if (!hWnd)
    {
        MessageBoxA(NULL, "Could not create the render window.", "Error", MB_OK | MB_ICONERROR);
        return nullptr;
    }

    std::shared_ptr<Window> pWindow = std::make_shared<MakeWindow>(hWnd, windowName, width, height, vSync);

    gs_windows.insert(std::map<HWND, std::shared_ptr<Window>>::value_type(hWnd, pWindow));
    gs_windowByName.insert(std::map<std::wstring, std::shared_ptr<Window>>::value_type(windowName, pWindow));

    return pWindow;
}

 void WindowManager::DestroyWindow(std::wstring& windowName)
{
	std::shared_ptr<Window> pWindow = FindWindow(windowName);
	if(pWindow) DestroyWindow(pWindow);
}

 void WindowManager::DestroyWindow(std::shared_ptr<Window> window)
{
	if(window) window->Destroy();
}

 std::shared_ptr<Window> WindowManager::FindWindow(const std::wstring& windowName)
{
	std::shared_ptr<Window> window;
    auto iter = gs_windowByName.find(windowName);
    if (iter != gs_windowByName.end())
    {
        window = iter->second;
    }

    return window;
}

 void WindowManager::Quit(int exitCode)
{
	PostQuitMessage(exitCode);
}

 Microsoft::WRL::ComPtr<ID3D12Device2> WindowManager::GetDevice() const
{
	 return m_d3d12Device;
}

 std::shared_ptr<CommandQueue> WindowManager::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type) const
{
	std::shared_ptr<CommandQueue> commandQueue;
    switch (type)
    {
    case D3D12_COMMAND_LIST_TYPE_DIRECT:
        commandQueue = m_directCommandQueue;
        break;
    case D3D12_COMMAND_LIST_TYPE_COMPUTE:
        commandQueue = m_computeCommandQueue;
        break;
    case D3D12_COMMAND_LIST_TYPE_COPY:
        commandQueue = m_copyCommandQueue;
        break;
    default:
        assert(false && "Invalid command queue type.");
    }

    return commandQueue;
}

 void WindowManager::Flush()
{
	m_directCommandQueue->Flush();
    m_computeCommandQueue->Flush();
    m_copyCommandQueue->Flush();
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> WindowManager::CreateDescriptorHeap(UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = type;
    desc.NumDescriptors = numDescriptors;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    desc.NodeMask = 0;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    ThrowIfFailed(m_d3d12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

    return descriptorHeap;
}

 UINT WindowManager::GetDescriptHandlerIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const
{
	return m_d3d12Device->GetDescriptorHandleIncrementSize(type);
}

// Remove a window from our window lists.
static void RemoveWindow(HWND hWnd)
{
    auto windowIter = gs_windows.find(hWnd);
    if (windowIter != gs_windows.end())
    {
        std::shared_ptr<Window> pWindow = windowIter->second;
        gs_windowByName.erase(pWindow->GetWindowName());
        gs_windows.erase(windowIter);
    }
}
#pragma managed(push, off)
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	std::shared_ptr<Window> pWindow;
	{
		auto iter = gs_windows.find(hwnd);
		if (iter != gs_windows.end())
		{
			pWindow = iter->second;
		}
	}

	if (pWindow)
	{
		switch (message)
		{
		case WM_PAINT:
			// Also fills up delta time
			pWindow->OnUpdate();
			pWindow->OnRender();
			break;
		case WM_LBUTTONDOWN:
			{
				float xPos = GET_X_LPARAM(lParam); 
				float yPos = GET_Y_LPARAM(lParam);
				pWindow->OnMouseButtonPressed(xPos, yPos);
			}
			break;
		break;
		case WM_MOUSEWHEEL:
			{
				float zDelta = ((int)(short)HIWORD(wParam)) / (float)WHEEL_DELTA;
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				 // Convert the screen coordinates to client coordinates.
            POINT clientToScreenPoint;
            clientToScreenPoint.x = x;
            clientToScreenPoint.y = y;
            ScreenToClient(hwnd, &clientToScreenPoint);
				pWindow->OnMouseWheel(zDelta);
			}
			break;
		case WM_MOUSEMOVE:
        {
            bool lButton = (wParam & MK_LBUTTON) != 0;
            

            int x = ((int)(short)LOWORD(lParam));
            int y = ((int)(short)HIWORD(lParam));

            pWindow->OnMouseMoved(lButton, x, y);
        }
			break;
		case WM_LBUTTONUP:
			{
				float xPos = GET_X_LPARAM(lParam); 
				float yPos = GET_Y_LPARAM(lParam);
				pWindow->OnMouseButtonReleased(xPos, yPos);
			}
			break;
		case WM_DESTROY:
			{
				// If a window is being destroyed, remove it from the 
				// window maps.
				RemoveWindow(hwnd);

				if (gs_windows.empty())
				{
					// If there are no more windows, quit the application.
					PostQuitMessage(0);
				}
			}
			break;
		default:
			return DefWindowProcW(hwnd, message, wParam, lParam);
		}
	}
	else
	{
		return DefWindowProcW(hwnd, message, wParam, lParam);
	}

	return 0;
}
