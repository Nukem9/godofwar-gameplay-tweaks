#pragma once

namespace IntroLogoHook
{

extern void (*orig_ShowHideSLI)(__int64 SLIVM, bool Show);
extern int (*orig_StartTimerSLI)(__int64 SLIVM, __int64 a2);

void hk_ShowHideSLI(__int64 SLIVM, bool Show);
int hk_StartTimerSLI(__int64 SLIVM, __int64 a2);

}