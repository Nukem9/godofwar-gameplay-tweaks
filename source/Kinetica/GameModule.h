#pragma once

namespace Kinetica
{

class GameModule
{
public:
	struct SLIFunctionHandler
	{
		const char *m_Name;							// 0x0
		int (*m_Callback)(void *, void *, void *);	// 0x8
		const char *m_ArgTypeString;				// 0x10
		uint32_t m_Unknown18;						// 0x18
		uint32_t m_Unknown1C;						// 0x1C
	};
	static_assert(sizeof(SLIFunctionHandler) == 0x20);

	struct SLIPropertyHandler
	{
		const char *m_Name;									// 0x0
		int (*m_Callback)(void *, bool, void *, void *);	// 0x8
		const char *m_TypeName;								// 0x10
		uint32_t m_Unknown18;								// 0x18
		uint32_t m_Unknown1C;								// 0x1C
	};
	static_assert(sizeof(SLIPropertyHandler) == 0x20);

	static Lazy<std::span<const SLIFunctionHandler>> SLIFunctions;
	static Lazy<std::span<const SLIPropertyHandler>> SLIProperties;
};

}