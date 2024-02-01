#include <spdlog/spdlog.h>
#include <fstream>
#include <cstdarg>

#include "LuaScriptHook.h"

using namespace std::filesystem;
using namespace Kinetica::Lua;
using namespace Kinetica::WadChunk;

namespace LuaScriptHook
{

void DumpLuaBytecodeToDisk(const std::string_view ScriptPath, const void *Bytecode, size_t BytecodeLength)
{
	spdlog::trace("Dumping bytecode...");

	// Convert to a native path, e.g.
	// "C:\Program Files (x86)\Steam\steamapps\common\GodOfWar\mods\lua\gameart\scripts\characters\athena00\main.lua.bin"
	path filePath = ModConfig::GetModRelativePath("lua");
	filePath.append(ScriptPath);
	filePath.concat(".bin");

	create_directories(filePath.parent_path());
	std::ofstream file(filePath, std::ios::binary);

	if (file.is_open())
		file.write(reinterpret_cast<const char *>(Bytecode), BytecodeLength);
}

std::vector<uint8_t> CompileLuaSourceFromDisk(const std::string_view ScriptPath)
{
	// Convert to a native path, e.g.
	// "C:\Program Files (x86)\Steam\steamapps\common\GodOfWar\mods\lua\gameart\scripts\characters\athena00\main.lua"
	path filePath = ModConfig::GetModRelativePath("lua");
	filePath.append(ScriptPath);

	std::ifstream file(filePath);

	if (!file.is_open())
	{
		// File doesn't exist or is inaccessible
		return {};
	}

	spdlog::trace("Compiling new source code...");
	auto luaState = luaL_newstate();

	if (!luaState)
	{
		// Failed to create main lua state
		return {};
	}

	// Read source code & try compiling
	std::vector<uint8_t> bytecodeBuffer;
	std::stringstream sourceCodeBuffer;

	sourceCodeBuffer << file.rdbuf();
	int compileStatus = luaL_loadstring(luaState, sourceCodeBuffer.str().c_str());

	if (compileStatus != 0)
	{
		// Compile failed. Error message is pushed on the top of the stack.
		size_t messageLength = 0;
		auto errorMessage = lua_tolstring(luaState, -1, &messageLength);

		spdlog::error("************** LUA SCRIPT COMPILE ERROR **************");
		spdlog::error("Script path: {}", filePath.string());
		spdlog::error("Error: {}", errorMessage);
		spdlog::error("******************************************************");
	}
	else
	{
		// Compile succeeded. Convert the in-memory state to an array of bytes.
		str_dump(luaState);

		size_t bytecodeLength = 0;
		auto bytecodeData = reinterpret_cast<const uint8_t *>(lua_tolstring(luaState, -1, &bytecodeLength));

		bytecodeBuffer.assign(bytecodeData, bytecodeData + bytecodeLength);
	}

	lua_close(luaState);
	return bytecodeBuffer;
}

void IFFProcessLuaScriptChunk(const IFFHeader *Header, const IFFLuaScriptChunkHeader *ScriptChunk)
{
	// Create a copy of the buffer and normalize paths ("%PROJ_PATH%/gameart/scripts/characters/athena00/main.lua")
	const std::string scriptPartialPath = [&]()
	{
		for (size_t i = 0;; i++)
		{
			if (ScriptChunk->m_Path[i] == '\0' || i >= std::size(ScriptChunk->m_Path))
			{
				std::string s(ScriptChunk->m_Path, i);
				std::transform(s.begin(), s.end(), s.begin(), [](char c) { return static_cast<char>(std::tolower(c)); });

				if (const auto index = s.find("%proj_path%/"); index != std::string::npos)
					s = s.substr(index + 12);

				return s;
			}
		}
	}();

	spdlog::info("Loading lua script [Context: {0:}, Path: {1:}]", Header->m_WadContext, scriptPartialPath);

	if (ModConfig::Lua::DumpScripts)
	{
		// Runtime equivalent of extracting individual wad files
		DumpLuaBytecodeToDisk(scriptPartialPath, ScriptChunk->m_Bytecode, Header->m_Length - sizeof(IFFLuaScriptChunkHeader));
	}

	if (ModConfig::Lua::LoadScripts)
	{
		auto newBytecode = CompileLuaSourceFromDisk(scriptPartialPath);

		if (!newBytecode.empty())
		{
			// Original header must be prepended
			auto headerData = reinterpret_cast<const uint8_t *>(ScriptChunk);
			newBytecode.insert(newBytecode.begin(), headerData, headerData + sizeof(IFFLuaScriptChunkHeader));

			// Continue with the newly compiled instance
			LoadLuaScriptChunk(Header->m_WadContext, newBytecode.data(), static_cast<uint32_t>(newBytecode.size()));
			return;
		}
	}

	// Continue with the original script data
	LoadLuaScriptChunk(Header->m_WadContext, ScriptChunk, Header->m_Length);
}

void LoadLuaScriptChunk(const char *WadContext, const void *RawData, uint32_t Length)
{
	return Offsets::CallID<"LoadLuaScriptChunk", void(*)(void *, const char *, const void *, uint32_t)>(nullptr, WadContext, RawData, Length);
}

void *hk_call_140459BA2(void *Unused, uint32_t Size, uint32_t Alignment)
{
	return _aligned_malloc(Size, Alignment);
}

lua_State *hk_call_1405A19BF(lua_Alloc f, void *ud)
{
	auto handleLuaAlloc = +[](void *UserData, void *BlockPtr, size_t OldSize, size_t NewSize) -> void *
	{
		if (NewSize == 0)
		{
			free(BlockPtr);
			return nullptr;
		}

		return realloc(BlockPtr, NewSize);
	};

	// 'f' is discarded
	return lua_newstate(handleLuaAlloc, ud);
}

void LuaServerLog(lua_State *State, int Level, const char *Format, ...)
{
	char buffer[4096];
	va_list va;

	va_start(va, Format);
	_vsnprintf_s(buffer, _TRUNCATE, Format, va);
	va_end(va);

	switch (Level)
	{
	case 1:
		spdlog::warn("{}", buffer);
		break;

	case 2:
		spdlog::error("{}", buffer);
		break;

	default:
		spdlog::info("{}", buffer);
		break;
	}
}

int LuaPrint(lua_State *State)
{
	char buffer[4096]{};
	const int argCount = lua_gettop(State);

	for (int i = 1; i <= argCount; i++)
	{
		size_t len = 0;
		auto str = luaL_tolstring(State, i, &len);

		if (str && len > 0)
		{
			strcat_s(buffer, str);

			// Each argument is separated by a space
			if (str[len - 1] != ' ')
				strcat_s(buffer, " ");
		}

		lua_pop(State, 1);
	}

	spdlog::info("{}", buffer);
	return 0;
}

int LuaGoldPrint(lua_State *State)
{
	return LuaPrint(State);
}

}
