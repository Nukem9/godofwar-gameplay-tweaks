#pragma once

#include <Kinetica/Lua.h>
#include <Kinetica/WadChunk.h>

namespace LuaScriptHook
{

void DumpLuaBytecodeToDisk(const std::string_view ScriptPath, const void *Bytecode, size_t BytecodeLength);
std::vector<uint8_t> CompileLuaSourceFromDisk(const std::string_view ScriptPath);
void IFFProcessLuaScriptChunk(const Kinetica::WadChunk::IFFHeader *Header, const Kinetica::WadChunk::IFFLuaScriptChunkHeader *ScriptChunk);
void LoadLuaScriptChunk(const char *WadContext, const void *RawData, uint32_t Length);
void *hk_call_140459BA2(void *Unused, uint32_t Size, uint32_t Alignment);
Kinetica::Lua::lua_State *hk_call_1405A19BF(Kinetica::Lua::lua_Alloc f, void *ud);

void LuaServerLog(Kinetica::Lua::lua_State *State, int Level, const char *Format, ...);
int LuaPrint(Kinetica::Lua::lua_State *State);
int LuaGoldPrint(Kinetica::Lua::lua_State *State);

}