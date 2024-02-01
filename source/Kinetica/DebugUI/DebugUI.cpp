#include <fstream>
#include <Windows.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

#include "../Camera/Blender.h"
#include "../goPlayer.h"
#include "../goSoldier.h"
#include "../Matrix.h"
#include "../Vector3.h"

#include "MainMenuBar.h"
#include "DebugUI.h"

// This has to be forward declared as ImGui avoids leaking Win32 types in header files
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Kinetica::DebugUI
{

bool UIInitialized = false;
bool InterceptGameInput = false;

std::unordered_map<std::string, std::shared_ptr<Window>> m_Windows;

void Initialize(HWND Window, ID3D11Device *Device, ID3D11DeviceContext *DeviceContext)
{
	if (!ModConfig::General::EnableDebugMenu)
		return;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	auto& style = ImGui::GetStyle();
	style.FrameBorderSize = 1.0f;
	style.ScrollbarRounding = 0.0f;

	auto& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.IniSavingRate = 15.0f;
	io.FontGlobalScale = ModConfig::General::DebugMenuFontScale;
	io.MouseDrawCursor = false;

	UpdateIniSettings(false);
	ImGui_ImplWin32_Init(Window);
	ImGui_ImplDX11_Init(Device, DeviceContext);

	UIInitialized = true;
	DebugUI::AddWindow(std::make_shared<MainMenuBar>());
}

void UpdateUI()
{
	if (!UIInitialized)
		return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	UpdateFreecam();

	// Clear window focus for every frame that isn't intercepting input
	if (!InterceptGameInput)
		ImGui::FocusWindow(nullptr);

	// A copy is required because Render() could create new instances and invalidate iterators
	for (const auto all = m_Windows; auto& [name, window] : all)
	{
		if (window->Close())
			m_Windows.erase(name);
		else
			window->Render();
	}

	// Handle INI disk updates
	if (auto& io = ImGui::GetIO(); io.WantSaveIniSettings)
	{
		UpdateIniSettings(true);
		io.WantSaveIniSettings = false;
	}

	ImGui::Render();
}

void RenderUI()
{
	if (!UIInitialized)
		return;

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

std::optional<LRESULT> HandleMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (!UIInitialized)
		return std::nullopt;

	switch (Msg)
	{
	case WM_KEYDOWN:
	{
		if (wParam == ModConfig::Hotkeys::ToggleDebugUI)
		{
			// Toggle input blocking and the main menu bar
			ToggleInputInterception();
			MainMenuBar::ToggleVisibility();

			return 1;
		}
		else if (wParam == ModConfig::Hotkeys::ToggleFreeflyCamera)
		{
			MainMenuBar::ToggleFreeflyCamera();
			return 1;
		}
		else if (wParam == ModConfig::Hotkeys::ToggleNoclip)
		{
			MainMenuBar::ToggleNoclip();
			return 1;
		}
	}
	break;
	}

	// Allow imgui to process it
	ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);

	if (ShouldInterceptInput())
	{
		const static std::unordered_set<UINT> blockedMessages
		{
			WM_MOUSEMOVE, WM_MOUSELEAVE, WM_LBUTTONDOWN, WM_LBUTTONDBLCLK, WM_RBUTTONDOWN, WM_RBUTTONDBLCLK,
			WM_MBUTTONDOWN, WM_MBUTTONDBLCLK, WM_XBUTTONDOWN, WM_XBUTTONDBLCLK, WM_LBUTTONUP, WM_RBUTTONUP,
			WM_MBUTTONUP, WM_XBUTTONUP, WM_MOUSEWHEEL, WM_MOUSEHWHEEL, WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN,
			WM_SYSKEYUP, WM_CHAR, WM_INPUT,
		};

		if (blockedMessages.contains(Msg))
			return 0;
	}

	// Return nullopt to indicate the game should process the message instead
	return std::nullopt;
}

void UpdateIniSettings(bool Save)
{
	auto iniPath = ModConfig::GetModRelativePath("loader_ui.cfg");

	if (Save)
	{
		std::ofstream f(iniPath);

		if (!f.is_open())
			return;

		size_t settingsDataLength = 0;
		auto settingsData = ImGui::SaveIniSettingsToMemory(&settingsDataLength);

		f.write(settingsData, settingsDataLength);
	}
	else
	{
		std::ifstream f(iniPath);
		std::stringstream settingsDataBuffer;

		if (!f.is_open())
			return;

		settingsDataBuffer << f.rdbuf();
		ImGui::LoadIniSettingsFromMemory(settingsDataBuffer.view().data(), settingsDataBuffer.view().length());
	}
}

void AddWindow(std::shared_ptr<Window> Handle)
{
	// Immediately discard duplicate window instances
	auto id = Handle->GetId();

	if (!m_Windows.contains(id))
		m_Windows.emplace(id, Handle);
}

void ToggleInputInterception()
{
	InterceptGameInput = !InterceptGameInput;

	if (InterceptGameInput)
		ImGui::GetIO().MouseDrawCursor = true;
	else
		ImGui::GetIO().MouseDrawCursor = false;
}

bool ShouldInterceptInput()
{
	return InterceptGameInput || MainMenuBar::m_FreeCamMode == MainMenuBar::FreeCamMode::Free;
}

void UpdateFreecam()
{
	auto cameraMode = MainMenuBar::m_FreeCamMode;

	if (cameraMode == MainMenuBar::FreeCamMode::Off)
		return;

	auto blender = camera::Blender::Instance();
	auto& io = ImGui::GetIO();

	if (!blender)
		return;

	// Set up the camera's rotation matrix
	Matrix cameraMatrix;

	if (cameraMode == MainMenuBar::FreeCamMode::Free)
	{
		// Convert mouse X/Y to yaw/pitch angles
		static float currentCursorX = 0.0f;
		static float currentCursorY = 0.0f;
		static float targetCursorX = 0.0f;
		static float targetCursorY = 0.0f;

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f))
		{
			targetCursorX += io.MouseDelta.x * 0.5f;
			targetCursorY += io.MouseDelta.y * 0.5f;
		}

		// Exponential decay view angle smoothing (https://stackoverflow.com/a/10228863)
		const double springiness = 60.0;
		const float mult = static_cast<float>(1.0 - std::exp(std::log(0.5) * springiness * io.DeltaTime));

		currentCursorX += (targetCursorX - currentCursorX) * mult;
		currentCursorY += (targetCursorY - currentCursorY) * mult;

		float degreesX = std::fmodf(currentCursorX, 360.0f);
		if (degreesX < 0) degreesX += 360.0f;
		float degreesY = std::fmodf(currentCursorY, 360.0f);
		if (degreesY < 0) degreesY += 360.0f;

		// Degrees to radians
		float yaw = degreesX * (3.14159f / 180.0f);
		float pitch = degreesY * (3.14159f / 180.0f);

		cameraMatrix = Matrix(-yaw, -pitch, 0.0f);
	}
	else if (cameraMode == MainMenuBar::FreeCamMode::Noclip)
	{
		cameraMatrix = blender->m_CameraMatrix;
	}

	// Scale camera velocity based on delta time
	float speed = io.DeltaTime * 5.0f;

	if (io.KeysDown[ImGuiKey_LeftShift] || io.KeysDown[ImGuiKey_RightShift])
		speed *= 10.0f;
	else if (io.KeysDown[ImGuiKey_LeftCtrl] || io.KeysDown[ImGuiKey_RightCtrl])
		speed /= 5.0f;

	// WSAD keys for movement
	Vector3F cameraPosition = MainMenuBar::m_FreeCamPosition.Position();

	if (io.KeysDown['W'])
		cameraPosition += cameraMatrix.Forward() * speed;

	if (io.KeysDown['S'])
		cameraPosition -= cameraMatrix.Forward() * speed;

	if (io.KeysDown['A'])
		cameraPosition -= cameraMatrix.Right() * speed;

	if (io.KeysDown['D'])
		cameraPosition += cameraMatrix.Right() * speed;

	cameraMatrix.Rows[3] = Vector4F(cameraPosition.X, cameraPosition.Y, cameraPosition.Z, 1.0f);
	MainMenuBar::m_FreeCamPosition = cameraMatrix;
}

}
