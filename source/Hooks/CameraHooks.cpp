#include <Kinetica/Camera/Blender.h>
#include <Kinetica/DebugUI/MainMenuBar.h>
#include <Kinetica/goPlayer.h>

#include "CameraHooks.h"

namespace CameraHooks
{

float (*orig_CameraObjectGetFov)(__int64 CameraObject);

float hk_CameraObjectGetFov(__int64 CameraObject)
{
	// WARNING: Game code expects this function to **PRESERVE R9**. It does not adhere
	// to standard calling convention rules. Thanks Microsoft.
	//
	// "Consider volatile registers destroyed on function calls unless
	// otherwise safety-provable by analysis such as whole program optimization."
	auto name = *(const char **)(CameraObject + 0x38);

	if (name[0] == 'o' &&
		name[1] == 'u' &&
		name[2] == 't' &&
		name[3] == 'p' &&
		name[4] == 'u' &&
		name[5] == 't' &&
		name[6] == '\0')
		return ModConfig::Gameplay::CameraFov;

	return orig_CameraObjectGetFov(CameraObject);
}

void hk_UpdateCameraBlenderMatrix(Kinetica::camera::Blender *Thisptr)
{
	Offsets::CallID<"camera::Blender::UpdateMatrix", void(*)(Kinetica::camera::Blender *)>(Thisptr);

	if (Kinetica::DebugUI::MainMenuBar::m_FreeCamMode == Kinetica::DebugUI::MainMenuBar::FreeCamMode::Free)
		Thisptr->m_CameraMatrix = Kinetica::DebugUI::MainMenuBar::m_FreeCamPosition;
}

void hk_UpdatePlayerPositionJob(__int64 a1, __int64 a2, __int64 a3)
{
	if (Kinetica::DebugUI::MainMenuBar::m_FreeCamMode == Kinetica::DebugUI::MainMenuBar::FreeCamMode::Noclip)
	{
		auto player = Kinetica::goPlayer::GetLocalPlayer();

		if (player && *reinterpret_cast<void **>(a1 + 0x918) == player->m_Soldier)
			*reinterpret_cast<Kinetica::Vector4F *>(a2 + 0x0) = Kinetica::DebugUI::MainMenuBar::m_FreeCamPosition.Rows[3];
	}

	Offsets::CallID<"UpdatePlayerPositionJob", void(*)(__int64, __int64, __int64)>(a1, a2, a3);
}

}