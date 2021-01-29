#pragma once

// Constants used for rendering and other stuff
#define MAX_LIGHTS 16

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>
#include <shellapi.h> // For CommandLineToArgvW


// The min/max macros conflict with like-named member functions.
// Only use std::min and std::max defined in <algorithm>.
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

// In order to define a function called CreateWindow, the Windows macro needs to
// be undefined.
#if defined(CreateWindow)
#undef CreateWindow
#endif

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>



#include <string>
#include <wrl/client.h>

// D3D12 extension library.
#include <d3dx12.h>

// STL Headers
#include <algorithm>
#include <cassert>
#include <chrono>
#include <map>
#include <memory>
#include <vector>


// Helper functions
#include <Helpers.h>
#include "HighResClock.h"
#include <WindowManager.h>
#include <CommandQueue.h>
#include <Window.h>
#include <Program.h>
#include <VertexStructs.h>
#include <Mesh3D.h>
#include <ObjReader.h>
#include <Camera.h>
#include <Light.h>
#include <LightManager.h>
#include <ArcBall.h>
#include <CDescriptorHeap.h>
#include <CUploadBuffer.h>
#include <TextureResource.h>
#include <ShaderResource.h>
#include <CCubeMap.h>
// Texture library
#include <DirectXTex.h>