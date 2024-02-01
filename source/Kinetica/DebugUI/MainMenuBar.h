#pragma once

#include "DebugUIWindow.h"

#include "../Matrix.h"

namespace Kinetica::DebugUI
{

class MainMenuBar : public Window
{
public:
	enum class FreeCamMode
	{
		Off,
		Free,
		Noclip,
	};

	static inline bool m_IsVisible;
	static inline FreeCamMode m_FreeCamMode;
	static inline Matrix m_FreeCamPosition;

	virtual void Render() override;
	virtual bool Close() override;
	virtual std::string GetId() const override;

	static void ToggleVisibility();
	static void ToggleFreeflyCamera();
	static void ToggleNoclip();

private:
	void DrawCheatsMenu();
	void DrawDebugMenu();
	void DrawMiscellaneousMenu();
};

}