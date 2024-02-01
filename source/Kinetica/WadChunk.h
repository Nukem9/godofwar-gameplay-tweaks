#pragma once

namespace Kinetica::WadChunk
{

#pragma warning(push)
#pragma warning(disable: 4200)

struct IFFHeader
{
	uint16_t m_Type;			// 0x0
	uint16_t m_Flags;			// 0x2
	uint32_t m_Length;			// 0x4
	char _pad8[0x10];			// 0x8 Likely GUID
	const char m_WadContext[72];// 0x18
};
static_assert(sizeof(IFFHeader) == 0x60);

struct IFFLuaScriptChunkHeader
{
	char m_Path[256];		// 0x0
	char m_Context[256];	// 0x100
	uint32_t m_Unknown200;	// 0x200
	uint8_t m_Bytecode[0];	// 0x204
};
static_assert(sizeof(IFFLuaScriptChunkHeader) == 0x204);

#pragma warning(pop)

}