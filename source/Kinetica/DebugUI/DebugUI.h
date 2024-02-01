#pragma once

#include <Windows.h>
#include <memory>
#include <optional>

#include "DebugUIWindow.h"

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace Kinetica::DebugUI
{

void Initialize(HWND Window, ID3D11Device *Device, ID3D11DeviceContext *DeviceContext);
void UpdateUI();
void RenderUI();
std::optional<LRESULT> HandleMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

void UpdateIniSettings(bool Save);
void AddWindow(std::shared_ptr<Window> Handle);
void ToggleInputInterception();
bool ShouldInterceptInput();
void UpdateFreecam();

}
