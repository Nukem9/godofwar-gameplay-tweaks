#pragma once

#include "../Common.h"
#include "../Position.h"
#include "../EffectParam.h"

namespace Kinetica
{

class CameraControl;
class CameraControlConstraints;
class CameraRecenter;
class CameraTransition;
class CameraTweenOverride;

class Camera : public EffectParm
{
public:
	enum eCameraType : uint8_t
	{
		kMinimalCamera = 0,
		kAnimatedCamera = 1,
		kDynamicCamera = 2,
		kTrackToFrameCamera = 3,
		kDebugCamera = 4,
		kCombatCamera = 5,
		kPlayerCamera = 6,
		kVehicleCamera = 7,
		kMaxCameraTypes = 8,
	};

	enum eStateFilters : uint16_t
	{
		kInCombat = 1,
		kOnGround = 2,
		kInAir = 4,
		kClimbing = 8,
		kJumping = 16,
		kFalling = 32,
		kHeartOfDarkness = 64,
		kRidingCreature = 128,
		kAttached = 256,
		kDead = 512,
		kTethered = 1024,
		kInVehicle = 2048,
	};

	enum eTargetFilterFlags : uint8_t
	{
		kLocalPlayerTarget = 2,
		kCreatureTargets = 4,
		kEnvironmentTargets = 8,
		kSameTeamTargets = 16,
		kOtherTeamTargets = 32,
	};

	const char *TemplateSymbol;// 0x40
	eCameraType CameraType;// 0x48
	bool CameraType_IsNull;// 0x49
	bool FieldBlended;// 0x4A
	bool FieldBlended_IsNull;// 0x4B
	bool ParentRelative;// 0x4C
	bool ParentRelative_IsNull;// 0x4D
	bool Aggression;// 0x4E
	bool Aggression_IsNull;// 0x4F
	bool TargetActivated;// 0x50
	bool TargetActivated_IsNull;// 0x51
	bool Collision;// 0x52
	bool Collision_IsNull;// 0x53
	bool DontCheckpoint;// 0x54
	bool DontCheckpoint_IsNull;// 0x55
	bool StartCentered;// 0x56
	bool StartCentered_IsNull;// 0x57
	bool RegisterName;// 0x58
	bool RegisterName_IsNull;// 0x59
	bool KeepActive;// 0x5A
	bool KeepActive_IsNull;// 0x5B
	int8_t ConstraintFriction;// 0x5C
	bool ConstraintFriction_IsNull;// 0x5D
	Position Position;// 0x60
	bool Position_IsNull;// 0x6C
	float Yaw;// 0x70
	bool Yaw_IsNull;// 0x74
	float Pitch;// 0x78
	bool Pitch_IsNull;// 0x7C
	float Roll;// 0x80
	bool Roll_IsNull;// 0x84
	float focalLength;// 0x88
	bool focalLength_IsNull;// 0x8C
	float AngleOfView;// 0x90
	bool AngleOfView_IsNull;// 0x94
	float FocusDistance;// 0x98
	bool FocusDistance_IsNull;// 0x9C
	float FStop;// 0xA0
	bool FStop_IsNull;// 0xA4
	float LensDistortion;// 0xA8
	bool LensDistortion_IsNull;// 0xAC
	bool DepthOfField;// 0xAD
	bool DepthOfField_IsNull;// 0xAE
	bool IsOrthographic;// 0xAF
	bool IsOrthographic_IsNull;// 0xB0
	float OrthoWidth;// 0xB4
	bool OrthoWidth_IsNull;// 0xB8
	eStateFilters StateFilter;// 0xBA
	bool StateFilter_IsNull;// 0xBC
	Array<StringHash> RequireMarker;// 0xC0
	bool RequireMarker_IsNull;// 0xCC
	Array<StringHash> IgnoreMarker;// 0xD0
	bool IgnoreMarker_IsNull;// 0xDC
	float DefaultTweenTime;// 0xE0
	bool DefaultTweenTime_IsNull;// 0xE4
	float DefaultEaseIn;// 0xE8
	bool DefaultEaseIn_IsNull;// 0xEC
	float DefaultEaseOut;// 0xF0
	bool DefaultEaseOut_IsNull;// 0xF4
	Array<CameraTweenOverride> TweenOverrides;// 0xF8
	bool TweenOverrides_IsNull;// 0x104
	Array<CameraTransition> Transition;// 0x108
	bool Transition_IsNull;// 0x114
	int16_t TargetCap;// 0x116
	bool TargetCap_IsNull;// 0x118
	int16_t MinimumTargetPriority;// 0x11A
	bool MinimumTargetPriority_IsNull;// 0x11C
	int16_t MinimumTargetActivatedPriority;// 0x11E
	bool MinimumTargetActivatedPriority_IsNull;// 0x120
	const char *TargetMatches;// 0x128
	bool TargetMatches_IsNull;// 0x130
	eTargetFilterFlags TargetFilter;// 0x131
	bool TargetFilter_IsNull;// 0x132
	float BoomDamping;// 0x134
	bool BoomDamping_IsNull;// 0x138
	float VerticalDamping;// 0x13C
	bool VerticalDamping_IsNull;// 0x140
	float HorizontalDamping;// 0x144
	bool HorizontalDamping_IsNull;// 0x148
	float MinVelocity;// 0x14C
	bool MinVelocity_IsNull;// 0x150
	float MaxVelocity;// 0x154
	bool MaxVelocity_IsNull;// 0x158
	float VelocityEaseIn;// 0x15C
	bool VelocityEaseIn_IsNull;// 0x160
	float VelocityEaseOut;// 0x164
	bool VelocityEaseOut_IsNull;// 0x168
	float VelocityDampingTime;// 0x16C
	bool VelocityDampingTime_IsNull;// 0x170
	Array<const char *> Effect;// 0x178
	bool Effect_IsNull;// 0x184
	CPtr<CameraControl> OrbitControl;// 0x188
	bool OrbitControl_IsNull;// 0x190
	CPtr<CameraControl> TiltControl;// 0x198
	bool TiltControl_IsNull;// 0x1A0
	CPtr<CameraControlConstraints> OrbitConstraint;// 0x1A8
	bool OrbitConstraint_IsNull;// 0x1B0
	CPtr<CameraControlConstraints> TiltConstraint;// 0x1B8
	bool TiltConstraint_IsNull;// 0x1C0
	CPtr<CameraRecenter> Recenter;// 0x1C8
	bool Recenter_IsNull;// 0x1D0
	CPtr<CameraRecenter> AutoRecenter;// 0x1D8
	bool AutoRecenter_IsNull;// 0x1E0
	const char *Script;// 0x1E8
	bool Script_IsNull;// 0x1F0
};
static_assert(offsetof(Camera, AngleOfView) == 0x90);
static_assert(offsetof(Camera, RequireMarker) == 0xC0);
static_assert(offsetof(Camera, RequireMarker_IsNull) == 0xCC);
static_assert(offsetof(Camera, IgnoreMarker) == 0xD0);
static_assert(offsetof(Camera, DefaultTweenTime_IsNull) == 0xE4);
static_assert(offsetof(Camera, TweenOverrides) == 0xF8);
static_assert(offsetof(Camera, Transition_IsNull) == 0x114);
static_assert(offsetof(Camera, BoomDamping_IsNull) == 0x138);
static_assert(sizeof(Camera) == 0x1F8);

}