#pragma once
#include "Rendering/ComPtr.h"
#include <d3d12.h>
#include <windows.h>
#include "d3dx12.h"
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"user32.lib") 
#pragma comment(lib,"Gdi32.lib") 
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"DirectXTK12.lib")

#define AppName "DirectX"

constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;    

class Main {
public:

	LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);       // メッセージプロシージャ関数
	
	HWND mHWnd;
};

