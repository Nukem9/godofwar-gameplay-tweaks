#include <spdlog/spdlog.h>
#include <Windows.h>

#include "LogHooks.h"

namespace LogHooks
{

void hk_OutputDebugStringA(const char *Text)
{
	spdlog::trace("{}", Text);
}

FILE *hk___acrt_iob_func(uint32_t Ix)
{
	const static auto iobfuncPtr = reinterpret_cast<decltype(&hk___acrt_iob_func)>(GetProcAddress(GetModuleHandleA("api-ms-win-crt-stdio-l1-1-0.dll"), "__acrt_iob_func"));

	return iobfuncPtr(Ix);
}

size_t hk_fwrite(const void *Buffer, size_t ElementSize, size_t ElementCount, FILE *Stream)
{
	const static auto fwritePtr = reinterpret_cast<decltype(&hk_fwrite)>(GetProcAddress(GetModuleHandleA("api-ms-win-crt-stdio-l1-1-0.dll"), "fwrite"));

	if (Stream != hk___acrt_iob_func(1))
		return fwritePtr(Buffer, ElementSize, ElementCount, Stream);

	// Intercept printfs to stdout
	std::string_view str(static_cast<const char *>(Buffer), ElementCount * ElementSize);

	if (str.contains("Unrecoverable Lua error"))
	{
		spdlog::critical("{}", str);
		MessageBoxA(nullptr, str.data(), "Error", MB_ICONERROR);
	}
	else
	{
		spdlog::info("{}", str);
	}

	return ElementCount;
}

int hk___stdio_common_vfprintf(void *Options, FILE *Stream, const char *Format, _locale_t Locale, va_list ArgList)
{
	const static auto vfprintfPtr = reinterpret_cast<decltype(&hk___stdio_common_vfprintf)>(GetProcAddress(GetModuleHandleA("api-ms-win-crt-stdio-l1-1-0.dll"), "__stdio_common_vfprintf"));

	if (Stream != hk___acrt_iob_func(1))
		return vfprintfPtr(Options, Stream, Format, Locale, ArgList);

	// Intercept printfs to stdout
	char buffer[4096];
	const int len = _vsnprintf_s(buffer, _TRUNCATE, Format, ArgList);

	hk_fwrite(buffer, sizeof(char), len, Stream);
	return len;
}

}