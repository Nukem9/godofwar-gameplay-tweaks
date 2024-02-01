#include "IntroLogoHook.h"

namespace IntroLogoHook
{

bool SkipTimers = true;

void (*orig_ShowHideSLI)(__int64 SLIVM, bool Show);
int (*orig_StartTimerSLI)(__int64 SLIVM, __int64 a2);

void hk_ShowHideSLI(__int64 SLIVM, bool Show)
{
	for (uint32_t i = 0; i < *(uint32_t *)(SLIVM + 636); i++)
	{
		auto stackTop = (*(uint32_t *)(SLIVM + 632) + i - *(uint32_t *)(SLIVM + 636));
		auto name = *(const char **)(SLIVM + 8i64 * stackTop + 56);

		if (name)
		{
			if (!_stricmp(name, "BootSiePresentsLogo") || !_stricmp(name, "Boot_SMS_Logo"))
				Show = false;

			if (!_stricmp(name, "deadScreen"))
				SkipTimers = false;
		}
	}

	orig_ShowHideSLI(SLIVM, Show);
}

int hk_StartTimerSLI(__int64 SLIVM, __int64 a2)
{
	auto stackTop = (*(uint32_t *)(SLIVM + 632) - *(uint32_t *)(SLIVM + 636));
	float& time = *(float *)(SLIVM + 8i64 * stackTop + 56);

	if (SkipTimers)
	{
		if (time > 0.0f)
			time = 0.01f;
	}

	return orig_StartTimerSLI(SLIVM, a2);
}

}