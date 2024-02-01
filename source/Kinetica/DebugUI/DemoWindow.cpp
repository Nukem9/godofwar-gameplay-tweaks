#include <imgui.h>

#include "DebugUI.h"
#include "DemoWindow.h"

namespace Kinetica::DebugUI
{

void DemoWindow::Render()
{
	ImGui::ShowDemoWindow(&m_WindowOpen);
}

bool DemoWindow::Close()
{
	return !m_WindowOpen;
}

std::string DemoWindow::GetId() const
{
	return "Dear Imgui Demo Window";
}

}