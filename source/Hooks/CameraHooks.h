#pragma once

namespace Kinetica::camera
{
class Blender;
}

namespace CameraHooks
{

extern float (*orig_CameraObjectGetFov)(__int64 CameraObject);

float hk_CameraObjectGetFov(__int64 CameraObject);
void hk_UpdateCameraBlenderMatrix(Kinetica::camera::Blender *Thisptr);
void hk_UpdatePlayerPositionJob(__int64 a1, __int64 a2, __int64 a3);

}