#include <imgui.h>

#include "../Camera/Blender.h"
#include "../goSoldier.h"
#include "../goPlayer.h"

#include "DebugUI.h"
#include "LogWindow.h"
#include "DemoWindow.h"
#include "MainMenuBar.h"

namespace Kinetica::DebugUI
{

void MainMenuBar::Render()
{
	if (!m_IsVisible)
		return;

	if (!ImGui::BeginMainMenuBar())
		return;

	// Empty space for MSI afterburner display
	if (ImGui::BeginMenu("                        ", false))
		ImGui::EndMenu();

	// "Cheats" menu
	if (ImGui::BeginMenu("Cheats"))
	{
		DrawCheatsMenu();
		ImGui::EndMenu();
	}

	// "Debug" menu
	if (ImGui::BeginMenu("Debug"))
	{
		DrawDebugMenu();
		ImGui::EndMenu();
	}

	// "Miscellaneous" menu
	if (ImGui::BeginMenu("Miscellaneous"))
	{
		DrawMiscellaneousMenu();
		ImGui::EndMenu();
	}

	// Credits
	auto text = ")9mekuN( mekuN yb detaerc doM | dekcolb tupni emaG";
	char buf[128]{};
	for (size_t i = 0; i < strlen(text); i++)
		buf[i] = text[strlen(text)-i-1];

	auto len = ImGui::CalcTextSize(buf).x;

	ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - len);
	ImGui::BeginMenu(buf, false);

	ImGui::EndMainMenuBar();
}

bool MainMenuBar::Close()
{
	return false;
}

std::string MainMenuBar::GetId() const
{
	return "Main Menu Bar";
}

void MainMenuBar::ToggleVisibility()
{
	m_IsVisible = !m_IsVisible;
}

void MainMenuBar::ToggleFreeflyCamera()
{
	auto blender = camera::Blender::Instance();

	if (!blender)
		return;

	m_FreeCamMode = (m_FreeCamMode == FreeCamMode::Free) ? FreeCamMode::Off : FreeCamMode::Free;
	m_FreeCamPosition = blender->m_CameraMatrix;
}

void MainMenuBar::ToggleNoclip()
{
	auto player = goPlayer::GetLocalPlayer();

	if (!player || !player->m_Soldier)
		return;

	m_FreeCamMode = (m_FreeCamMode == FreeCamMode::Noclip) ? FreeCamMode::Off : FreeCamMode::Noclip;
	m_FreeCamPosition = player->m_Soldier->m_Matrix;
}

void MainMenuBar::DrawCheatsMenu()
{
	if (ImGui::MenuItem("Enable Freefly Camera", nullptr, m_FreeCamMode == FreeCamMode::Free))
		ToggleFreeflyCamera();

	if (ImGui::MenuItem("Enable Noclip", nullptr, m_FreeCamMode == FreeCamMode::Noclip))
		ToggleNoclip();
}

void MainMenuBar::DrawDebugMenu()
{
	if (ImGui::MenuItem("Show Log Window"))
		AddWindow(std::make_shared<LogWindow>());
}

void MainMenuBar::DrawMiscellaneousMenu()
{
	if (ImGui::MenuItem("Show ImGui Demo Window"))
		AddWindow(std::make_shared<DemoWindow>());

	ImGui::Separator();
	ImGui::MenuItem("##miscemptyspace1", nullptr, nullptr, false);
	ImGui::MenuItem("##miscemptyspace2", nullptr, nullptr, false);
	ImGui::Separator();

	if (ImGui::MenuItem("Terminate Process"))
		TerminateProcess(GetCurrentProcess(), 0);
}

}
