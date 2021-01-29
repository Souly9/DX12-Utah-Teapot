#pragma once

class Window;
class Program;
class CommandQueue;

/**
 * Class that holds X amount of windows and manages them
 * Also responsible for DirectX12 device creation and to submit incoming command lists
 */
class WindowManager
{
public:

	/**
	 * Creates the Manager one time
	 */
	static void Create(HINSTANCE hInst);

	/**
	 * Destroy the Manager along all its windows
	 */
	static void Destroy();

	/**
	 * Singleton that returns a reference to this class
	 */
	static WindowManager& Get();

	/**
	 * Creates a new Window DirectX12 can render to
	 * @param windowName The name of the window that will appear in the title bar
	 * @param width The window width
	 * @param height The window height
	 * @returns A shared pointer to the created window instance
	 * If a window with the same name already exists, its handle will be returned
	 */
	std::shared_ptr<Window> CreateRenderWindow(const std::wstring& windowName, int width, int height, bool vSync);

	/**
	 * Destroy a window by name
	 */
	void DestroyWindow(std::wstring& windowName);
	/**
	 * Destroy a window by reference
	 */
	void DestroyWindow(std::shared_ptr<Window> window);

	/**
	 * Find a window by name!
	 */
	std::shared_ptr<Window> FindWindow(const std::wstring& windowName);

	/**
	 * Run the window loop and message system
	 * @return Error code when run failed
	 */
	int Run(std::shared_ptr<Program> pProgram);

	/**
	 * Tell the Manager to close all windows and applications
	 * @param exitCode The error code to return to the invoker
	 */
	void Quit(int exitCode = 0);

	/**
	 * Returns the DirectX12 device
	 */
	Microsoft::WRL::ComPtr<ID3D12Device2> GetDevice() const;
	/**
	 * Returns the command queue
	 * @param type The D3D12_COMMAND_LIST_TYPE, valid types are
	 * - D3D12_COMMAND_LIST_TYPE_DIRECT : Can be used for draw, dispatch, or copy commands.
     * - D3D12_COMMAND_LIST_TYPE_COMPUTE: Can be used for dispatch or copy commands.
     * - D3D12_COMMAND_LIST_TYPE_COPY   : Can be used for copy commands.
	 */
	std::shared_ptr<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;

	//Flush the command queues
	void Flush();

	//Descriptor Heap functions 	
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type);
	UINT GetDescriptHandlerIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const;
	

protected:
	//classic constructor
	WindowManager(HINSTANCE hInst);

	virtual ~WindowManager();

	Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter();
	Microsoft::WRL::ComPtr<ID3D12Device2> CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter);

private:
	WindowManager(const WindowManager& copy) = delete;
	WindowManager& operator=(const WindowManager& other) = delete;

	// The handle used to create the manager
	HINSTANCE m_hInstance;

	Microsoft::WRL::ComPtr<IDXGIAdapter4> m_dxgiAdapter;
	Microsoft::WRL::ComPtr<ID3D12Device2> m_d3d12Device;

	std::shared_ptr<CommandQueue> m_directCommandQueue;
	std::shared_ptr<CommandQueue> m_computeCommandQueue;
	std::shared_ptr<CommandQueue> m_copyCommandQueue;
};


