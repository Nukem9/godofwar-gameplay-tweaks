#pragma once

#include <Windows.h>

struct IDXGISwapChain;
struct ID3D11Device;

namespace Kinetica::Renderer
{

extern WNDPROC OriginalOnWindowMessage;

HWND GetWindow();
ID3D11Device *GetDevice();

HRESULT WINAPI OnPresent(IDXGISwapChain *SwapChain, UINT SyncInterval, UINT Flags);
LRESULT WINAPI OnWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

}
