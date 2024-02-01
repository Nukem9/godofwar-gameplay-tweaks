#pragma once

#include "../Common.h"

namespace Kinetica::camera
{

class Blender
{
public:
	virtual void UnknownBlender01();
	virtual void UnknownBlender02();

	char _pad0[0xA98];
	Matrix m_CameraMatrix;
	Matrix m_UnknownCameraMatrix;

	static Blender *Instance()
	{
		return *Offsets::ResolveID<"gCameraBlender", Blender **>();
	}
};
//static_assert(sizeof(Blender) == 0x1300);

}