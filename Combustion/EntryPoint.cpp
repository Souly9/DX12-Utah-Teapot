#include <stdafx.h>
#include <Shlwapi.h>
#include <Combustion.h>
#include <dxgidebug.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	int size = sizeof(Vertex);
	WCHAR path[MAX_PATH];
    HMODULE hModule = GetModuleHandleW(NULL);
    if ( GetModuleFileNameW(hModule, path, MAX_PATH) > 0 )
    {
        PathRemoveFileSpecW(path);
        SetCurrentDirectoryW(path);
    }
	WindowManager::Create(hInstance);
	std::shared_ptr<Combustion> application = std::make_shared<Combustion>(L"Combustion Renderer", 2560, 1440, true);
	
	int retcode = WindowManager::Get().Run(application);

	return retcode;
}
