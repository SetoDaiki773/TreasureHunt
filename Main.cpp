#include "Main.h"
#include "Scene/Sequence.h"
#include "Rendering/Renderer.h"
#include "Manager/TextureManager.h"
#include <memory>
#include <string>
#include <cassert>
#include <iostream>
#include <vector>
#include <sstream>

#define TextureDimension 2048

using namespace DirectX;

namespace {
	Main* gMain;
}

// メッセージループ
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#if defined(_DEBUG)
int main()
#else
INT WINAPI WinMain(HINSTANCE , HINSTANCE, LPSTR, INT)
#endif
{

// メモリリークチェック
#if defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
#endif

	gMain = new Main();
	
	if (gMain != NULL)
	{
		//ウィンドウの作成
		WNDCLASSEX  wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
		wc.lpszClassName = (LPSTR)AppName;
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		RegisterClassEx(&wc);

		gMain->mHWnd = CreateWindow((LPSTR)AppName, (LPSTR)AppName, WS_OVERLAPPEDWINDOW,
			0, 0, WindowWidth, WindowHeight, 0, 0,wc.hInstance, 0);
		if (!gMain->mHWnd)
		{
			return E_FAIL;
		}

		ShowWindow(gMain->mHWnd, SW_SHOW);
		UpdateWindow(gMain->mHWnd);
		TextureManager::Init(&gMain->mHWnd);
		std::unique_ptr<Sequence> sequence = std::make_unique<Sequence>();
		// メッセージループ
		MSG msg = { 0 };
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT)
		{
			
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			
			else {
				//FPS計算表示
				static DWORD time = 0;
				static int frame = 0;
				frame++;
				std::stringstream frameRate;
				frameRate << "fps = " << frame;
				if (timeGetTime() - time > 1000)
				{
					time = timeGetTime();
					frame = 0;
					SetWindowTextA(gMain->mHWnd, frameRate.str().c_str());
				}
				sequence->RunScene();
			}
			
		}
	
	}
	delete gMain;
	
	return 0;
}


//------------------------------
//  ウィンドウプロシージャー
//------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return gMain->MsgProc(hWnd, uMsg, wParam, lParam);
}
LRESULT Main::MsgProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_KEYDOWN:
		switch ((char)wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}