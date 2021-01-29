#pragma once
#include <queue>


class CommandQueue
{
public:
	CommandQueue(Microsoft::WRL::ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
	virtual ~CommandQueue();

	// Returns a ready to use command list
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> GetCommandList();

	// Execute the issued commands
	uint64_t ExecuteCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);

	uint64_t Signal();
	bool IsFenceComplete(uint64_t fenceValue);
	void WaitForFenceValue(uint64_t fenceValue);
	void Flush();

	// Used to access the underlying command queue interface
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue();

protected:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator);

private:
	// Helper struct to keep track of all command allocators
	struct CommandAllocatorEntry
	{
		uint64_t fenceValue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	};

	D3D12_COMMAND_LIST_TYPE m_commandListType;
	Microsoft::WRL::ComPtr<ID3D12Device2> m_d3d12Device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_d3d12Fence;
	HANDLE m_fenceEvent;
	uint64_t m_fenceValue;

	std::queue<CommandAllocatorEntry> m_commandAllocatorQueue;
	std::queue<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>> m_commandListQueue;
};
