#include <dxgi.h>
#include <d3d11.h>

#include "DebugUI/DebugUI.h"
#include "Renderer.h"

namespace Kinetica::Renderer
{

WNDPROC OriginalOnWindowMessage;

HWND GetWindow()
{
	return *Offsets::ResolveID<"Renderer::WindowHandle", HWND *>();
}

ID3D11Device *GetDevice()
{
	return *Offsets::ResolveID<"Renderer::D3D11Device", ID3D11Device **>();
}

HRESULT WINAPI OnPresent(IDXGISwapChain *SwapChain, UINT SyncInterval, UINT Flags)
{
	static bool uiInit = [&]()
	{
		ID3D11Device *device = GetDevice();
		ID3D11DeviceContext *immediateContext = nullptr;

		device->GetImmediateContext(&immediateContext);
		DebugUI::Initialize(GetWindow(), device, immediateContext);
		immediateContext->Release();

		return true;
	}();

	DebugUI::UpdateUI();
	DebugUI::RenderUI();

	return SwapChain->Present(SyncInterval, Flags);
}

LRESULT WINAPI OnWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	auto handled = DebugUI::HandleMessage(hWnd, Msg, wParam, lParam);

	if (handled)
		return handled.value();

	return CallWindowProcA(OriginalOnWindowMessage, hWnd, Msg, wParam, lParam);
}

}
