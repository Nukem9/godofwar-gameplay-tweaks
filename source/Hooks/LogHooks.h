#pragma once

namespace LogHooks
{

void hk_OutputDebugStringA(const char *Text);
FILE *hk___acrt_iob_func(uint32_t Ix);
size_t hk_fwrite(const void *Buffer, size_t ElementSize, size_t ElementCount, FILE *Stream);
int hk___stdio_common_vfprintf(void *Options, FILE *Stream, const char *Format, _locale_t Locale, va_list ArgList);

}