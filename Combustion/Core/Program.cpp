#include <stdafx.h>


Program::Program(const std::wstring& name, int width, int height, bool vSync)
	: m_width(width)
	, m_height(height)
	, m_name( name)
	, m_vSync(vSync)
{
}

Program::~Program()
{
	assert(!m_pWindow && "Use Game::Destroy() before destruction.");
}

bool Program::Initialize()
{
    // Check for DirectX Math library support.
    if (!DirectX::XMVerifyCPUSupport())
    {
        MessageBoxA(NULL, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
        return false;
    }
 
    m_pWindow = WindowManager::Get().CreateRenderWindow(m_name, m_width, m_height, m_vSync);
    m_pWindow->RegisterCallbacks(shared_from_this());
    m_pWindow->Show();
 
    return true;
}

void Program::OnUpdate(float deltaTime, float totalTime)
{
}

void Program::OnRender(float deltaTime, float totalTime)
{
}

void Program::Destroy()
{
	WindowManager::Get().DestroyWindow(m_pWindow);
	m_pWindow.reset();
}

void Program::OnKeyReleased(WPARAM uint)
{
}

void Program::OnKeyPressed(WPARAM uint)
{
}

void Program::OnMouseWheel(float zDelta)
{
}

void Program::OnMouseButtonPressed(int x, int y)
{
}
void Program::OnMouseButtonReleased(int x, int y)
{
}

void Program::OnWindowDestroy()
{
}




