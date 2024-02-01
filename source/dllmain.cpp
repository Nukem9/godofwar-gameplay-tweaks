#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>

#include <Hooks/LogHooks.h>
#include <Hooks/LuaScriptHook.h>
#include <Hooks/LogHooks.h>
#include <Hooks/IntroLogoHook.h>
#include <Hooks/CameraHooks.h>
#include <Kinetica/Renderer.h>
#include <Kinetica/DebugUI/LogWindow.h>
#include <RTTI/MSRTTI.h>

#include "NewlineSplitterDistSink.h"
#include "XUtil.h"

void SetupLogging()
{
	auto newlineSplitterSink = std::make_shared<NewlineSplitterDistSinkMT>();

	// File sink
	if (auto logLevel = spdlog::level::from_str(ModConfig::Advanced::FileLogLevel); logLevel != spdlog::level::level_enum::off)
	{
		auto filePath = ModConfig::GetModRelativePath("loader_log.txt");
		auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filePath.string(), true);
		fileSink->set_level(logLevel);

		newlineSplitterSink->add_sink(std::move(fileSink));
	}

	// Console sink
	if (auto logLevel = spdlog::level::from_str(ModConfig::Advanced::ConsoleLogLevel); logLevel != spdlog::level::level_enum::off)
	{
		auto debugUISink = std::make_shared<Kinetica::DebugUI::LogWindow::LogSink>();
		debugUISink->set_level(logLevel);

		newlineSplitterSink->add_sink(std::move(debugUISink));
	}

	// Main logger
	auto logger = std::make_shared<spdlog::logger>("main_logger", std::move(newlineSplitterSink));
	logger->set_level(spdlog::level::level_enum::trace);
	logger->set_pattern("[%H:%M:%S] [%l] %v");
	logger->flush_on(spdlog::level::warn);

	spdlog::set_default_logger(std::move(logger));
	spdlog::flush_every(std::chrono::seconds(5));
}

void LoadSignatures()
{
	auto [moduleBase, moduleEnd] = Offsets::GetModule();

	// TODO: Switch to dedicated global variables?
	auto offsetFromOperand = [&](const char *Signature, uint32_t Add)
	{
		auto addr = XUtil::FindPattern(moduleBase, moduleEnd - moduleBase, Signature);

		if (!addr)
			return addr;

		return static_cast<uintptr_t>(*reinterpret_cast<uint32_t *>(addr + Add));
	};

	auto offsetFromRIP = [&](const char *Signature, uint32_t Add)
	{
		auto addr = XUtil::FindPattern(moduleBase, moduleEnd - moduleBase, Signature);

		if (!addr)
			return addr;

		auto relOffset = *reinterpret_cast<int32_t *>(addr + Add) + sizeof(int32_t);
		return addr + Add + relOffset - moduleBase;
	};

	// Functions
	Offsets::MapSignature("wadLoader::OpenFile", "48 89 5C 24 08 57 48 83 EC 30 8B ? ? ? ? ? 33 FF C1 E8 02 48 8B D9 A8 01");
	Offsets::MapSignature("dc::LookupSymByHash", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 20 41 8B E8 4C 8B F2 48 8B D9");
	Offsets::MapSignature("IFFProcessLuaScriptChunk", "44 8B 49 04 4C 8B C2 48 8D 51 18 E9 ? ? ? ?");
	Offsets::MapSignature("LoadLuaScriptChunk", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 48 89 7C 24 20 41 54 41 55 41 56 48 83 EC 20 48 8B");
	Offsets::MapSignature("lua_newstate", "40 53 56 57 48 83 EC 50 48 8B ? ? ? ? ? 48 33 C4 48 89 44 24 48");
	Offsets::MapSignature("lua_close", "48 8B 49 18 48 8B 89 08 01 00 00 E9 ? ? ? ?");
	Offsets::MapSignature("lua_load", "4C 8B DC 56 48 83 EC 50 33 C0 4D 89 43 E0 4D 85 C9 49 89 53 D8");
	Offsets::MapSignature("lua_tolstring", "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 49 8B D8 8B F2 48 8B F9 E8 ? ? ? ? 4C");
	Offsets::MapSignature("luaL_newstate", "48 83 EC 28 33 D2 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 85 C0");
	Offsets::MapSignature("luaL_tolstring", "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 8D 82 E7 07 00 00");
	Offsets::MapSignature("str_dump", "48 89 7C 24 18 55 48 8D AC 24 80 E0 FF FF B8 80 20 00 00 E8 ? ? ? ? 48 2B E0");
	Offsets::MapSignature("index2addr", "4C 8B 49 20 4C 8B C1 85 D2 7E 18 48 63 C2 48 C1 E0 04 49 03 01");
	Offsets::MapSignature("GameModule::ShowHideSLI", "40 55 56 41 56 41 57 48 83 EC 28 45 33 FF 0F B6 EA");
	Offsets::MapSignature("GameModule::StartTimerSLI", "48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 41 56 48 83 EC 20 8B 81 78 02 00 00 48 8B F1 44 8B 81 7C 02 00 00");
	Offsets::MapSignature("Renderer::OnWindowMessage", "40 55 53 56 57 41 55 41 56 41 57 48 8D 6C 24 D0 48 81 EC 30 01 00 00");
	Offsets::MapSignature("CameraObjectGetFov", "0F B6 ? ? ? ? ? 41 B8 01 00 00 00 84 C0 8B D0 41 0F 44 D0 80 3D ? ? ? ? ? 88 ? ? ? ? ? 74 09");
	Offsets::MapSignature("camera::Blender::UpdateMatrix", "48 8B C4 57 48 81 EC A0 00 00 00 48 89 58 10 48 8B F9 48 89 68 18 48 89 70 20 4C 89 60 F0");
	Offsets::MapSignature("UpdatePlayerPositionJob", "48 89 5C 24 20 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 D9 48 81 EC F0 00 00 00");

	// Hooks
	Offsets::MapSignature("LuaScriptChunkMemoryAllocatorHookLoc", "79 04 33 C9 EB 08 48 8D 0C CA 4A 8B 0C 21 41 B8 08 00 00 00 8B D3", 0x16);
	Offsets::MapSignature("LuaScriptAllocatorHookLoc", "E8 ? ? ? ? 49 89 46 58 4D 8B 46 28 0F B6 15");
	Offsets::MapSignature("RendererOnPresentHookLoc", "48 8B 01 FF 50 40 8B F0 85 C0 0F 89");
	Offsets::MapSignature("DisableDepthOfFieldPatchLoc", "41 22 BE AD 00 00 00 F3 41 0F 10 B6 98 00 00 00 F3 0F 59 C1 F3 0F 11");
	Offsets::MapSignature("DisableWindowsVersionCheckPatchLoc", "40 55 48 8D AC 24 80 FE FF FF 48 81 EC 80 02 00 00 48 8B");
	Offsets::MapSignature("PhotomodeCameraRestrictPatchLoc1", "0F 28 ? ? ? ? ? 66 0F 7F 45 F0 EB 12 48 8B");
	Offsets::MapSignature("PhotomodeCameraRestrictPatchLoc2", "0F 10 80 C0 08 00 00 0F 29 45 F0 F3 0F 10 ? ? ? ? ? 48 8D 45 F0");
	Offsets::MapSignature("PhotomodeCameraRestrictPatchLoc3", "0F 29 55 F0 74 14 48 8B");
	Offsets::MapSignature("PhotomodeCameraRestrictPatchLoc4", "75 10 41 0F 10 40 30 F3 41 0F 7F 45 00 E9 ? ? ? ? 80 65 7C FE");
	Offsets::MapAddress("LuaServerLogHookLoc", offsetFromRIP("E8 ? ? ? ? 48 83 43 10 F0 4C 8D 5C 24 70 49 8B 5B 20 32 C0", 0x1));
	Offsets::MapAddress("LuaPrintHookLoc", offsetFromRIP("48 8D ? ? ? ? ? BA FD FF FF FF 48 89 01 0F B6 40 08 83 C8 40", 0x3));
	Offsets::MapSignature("LuaGoldPrintHookLoc", "C7 05 ? ? ? ? ? ? ? ? 48 83 C4 28 C3 CC CC CC CC CC CC CC CC CC CC CC 33 C0 C3", 0x1A);
	Offsets::MapSignature("UpdateCameraBlenderMatrixHookLoc", "E8 ? ? ? ? 8B 86 48 08 00 00 BF 01 00 00 00 3B C7 0F 86");
	Offsets::MapSignature("UpdatePlayerPositionHookLoc", "E8 ? ? ? ? 90 48 8B 44 24 50 8B 4C 24 30 89 48 0C");

	// Globals
	Offsets::MapAddress("RTTI::HashMapAttributeTable", offsetFromRIP("4C 8D 2D ? ? ? ? 33 ED 4D 8B E1 49 8B F8 4C 8D 14 40 4F 63 74 D5 00", 0x3));
	Offsets::MapAddress("RTTI::ArrayAttributeTable", offsetFromOperand("8B 94 C8 ? ? ? ? 85 D2 75 12 41 0F 10 01 0F 29 44 24 20", 0x3));
	Offsets::MapAddress("RTTI::TypeAttributeTable", offsetFromRIP("48 8D 0D ? ? ? ? 48 89 44 24 28 66 66 0F 1F 84 00 00 00 00 00", 0x3));
	Offsets::MapAddress("RTTI::TypeDeclTable", offsetFromRIP("48 8D 05 ? ? ? ? 4E 8B 4C C8 30 E8 DB EF FF FF EB 50 3C 07", 0x3));
	Offsets::MapAddress("RTTI::EnumDeclTable", offsetFromOperand("4A 63 8C 00 ? ? ? ? 42 8B 94 00 ? ? ? ? 03 D1 3B CA 0F 84 CA 01 00 00", 0x4) - 0x8);
	Offsets::MapAddress("GameModule::SLIFunctionTable", offsetFromRIP("48 8D 0D ? ? ? ? FF 15 ? ? ? ? 4C 8D ? ? ? ? ? 41 8D 57 56", 0x3));
	Offsets::MapAddress("GameModule::SLIPropertyTable", offsetFromRIP("48 8D 0D ? ? ? ? FF 15 ? ? ? ? 65 48 8B 04 25 58 00 00 00 48 8B 10", 0x3));
	Offsets::MapAddress("Renderer::WindowHandle", offsetFromRIP("48 89 ? ? ? ? ? FF 15 ? ? ? ? 48 8B ? ? ? ? ? 48 8D 55 F7", 0x3));
	Offsets::MapAddress("Renderer::D3D11Device", offsetFromRIP("4C 8D ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ? 4C 8D ? ? ? ? ? 48 C7 05 ? ? ? ? ? ? ? ? 41 8B DC", 0x3));
	Offsets::MapAddress("gCameraBlender", offsetFromRIP("48 8B ? ? ? ? ? 80 B8 4F 08 00 00 00 74 0B 0F 57 C0", 0x3));
	Offsets::MapAddress("gLocalPlayerGO", offsetFromRIP("48 8B ? ? ? ? ? 48 85 D2 74 19 48 8B 42 08 4C 8B 40 08 B8 00 00 00 00", 0x3));
}

void ApplyHooks()
{
	auto [moduleBase, _] = Offsets::GetModule();

	MSRTTI::Initialize();

	//
	// Hook lua chunk loading:
	//  - Inject new scripts by intercepting the chunk load function and compiling/loading them on the fly
	//
	Offsets::WriteJump<"IFFProcessLuaScriptChunk">(&LuaScriptHook::IFFProcessLuaScriptChunk);

	//
	// The engine has a predetermined amount of memory reserved for lua scripts. This needs to be bypassed:
	//  - Patch the chunk allocator(s) for WAD files
	//  - Patch the call to lua_newstate()
	//
	Offsets::WriteCall<"LuaScriptChunkMemoryAllocatorHookLoc">(&LuaScriptHook::hk_call_140459BA2);
	Offsets::WriteCall<"LuaScriptAllocatorHookLoc">(&LuaScriptHook::hk_call_1405A19BF);

	//
	// Redirect game log prints:
	//  - Generic Lua functions (print, GoldPrint)
	//  - OutputDebugStringA calls
	//  - Writes to stdout
	//
	Offsets::WriteJump<"LuaServerLogHookLoc">(&LuaScriptHook::LuaServerLog);
	Offsets::WriteJump<"LuaPrintHookLoc">(&LuaScriptHook::LuaPrint);
	Offsets::WriteJump<"LuaGoldPrintHookLoc">(&LuaScriptHook::LuaGoldPrint);

	Offsets::RedirectImport(moduleBase, "KERNEL32.dll", "OutputDebugStringA", &LogHooks::hk_OutputDebugStringA, nullptr);
	Offsets::RedirectImport(moduleBase, "api-ms-win-crt-stdio-l1-1-0.dll", "__acrt_iob_func", &LogHooks::hk___acrt_iob_func, nullptr);
	Offsets::RedirectImport(moduleBase, "api-ms-win-crt-stdio-l1-1-0.dll", "fwrite", &LogHooks::hk_fwrite, nullptr);
	Offsets::RedirectImport(moduleBase, "api-ms-win-crt-stdio-l1-1-0.dll", "__stdio_common_vfprintf", &LogHooks::hk___stdio_common_vfprintf, nullptr);

	// Rendering
	Offsets::WriteCall<"RendererOnPresentHookLoc">(&Kinetica::Renderer::OnPresent);

	// Window messages
	Kinetica::Renderer::OriginalOnWindowMessage = Offsets::WriteJump<"Renderer::OnWindowMessage">(&Kinetica::Renderer::OnWindowMessage);

	// Intro logo skip
	if (ModConfig::Gameplay::SkipIntroLogos)
	{
		IntroLogoHook::orig_ShowHideSLI = Offsets::WriteJump<"GameModule::ShowHideSLI">(&IntroLogoHook::hk_ShowHideSLI);
		IntroLogoHook::orig_StartTimerSLI = Offsets::WriteJump<"GameModule::StartTimerSLI">(&IntroLogoHook::hk_StartTimerSLI);
	}

	// Depth of field
	if (ModConfig::Gameplay::RemoveDepthOfField)
		Offsets::WriteBytes<"DisableDepthOfFieldPatchLoc">({ 0x33, 0xFF, 0x90, 0x90, 0x90, 0x90, 0x90 });

	// Photomode camera unlock
	if (ModConfig::Gameplay::UnlockPhotomodeCamera)
	{
		Offsets::WriteBytes<"PhotomodeCameraRestrictPatchLoc1">({ 0x41, 0x0F, 0x28, 0xC0, 0x90, 0x90, 0x90 });
		Offsets::WriteBytes<"PhotomodeCameraRestrictPatchLoc2">({ 0x41, 0x0F, 0x28, 0xC0, 0x90, 0x90, 0x90 });
		Offsets::WriteBytes<"PhotomodeCameraRestrictPatchLoc3">({ 0x90, 0x90, 0x90, 0x90 });
		Offsets::WriteBytes<"PhotomodeCameraRestrictPatchLoc4">({ 0x90, 0x90 });
	}

	// FOV override
	if (ModConfig::Gameplay::OverrideCameraFov)
	{
		Offsets::WriteBytes<"CameraObjectGetFov">({ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
		CameraHooks::orig_CameraObjectGetFov = Offsets::WriteJump<"CameraObjectGetFov">(&CameraHooks::hk_CameraObjectGetFov);
	}

	// Freecam and noclip
	Offsets::WriteCall<"UpdateCameraBlenderMatrixHookLoc">(&CameraHooks::hk_UpdateCameraBlenderMatrix);
	Offsets::WriteCall<"UpdatePlayerPositionHookLoc">(&CameraHooks::hk_UpdatePlayerPositionJob);

	// Windows 10 version check
	if (ModConfig::Advanced::RemoveWindowsVersionCheck)
		Offsets::WriteBytes<"DisableWindowsVersionCheckPatchLoc">({ 0xB0, 0x01, 0xC3 });
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// It's extremely unlikely that a path will be this long, but it's a one time check, so who cares.
		std::wstring fullModulePath(32768, '\0');
		uint32_t len = GetModuleFileNameW(GetModuleHandleW(nullptr), fullModulePath.data(), static_cast<DWORD>(fullModulePath.size()));
		
		fullModulePath.resize(len);

		try
		{
			if (fullModulePath.empty())
				throw std::runtime_error("Unable to obtain executable path via GetModuleFileNameW()");

			ModConfig::Initialize(std::filesystem::path(fullModulePath).remove_filename());
			SetupLogging();
			LoadSignatures();
			ApplyHooks();
		}
		catch (const std::exception& e)
		{
			// Use a hardcoded buffer. std::format can't convert between wchar_t and char.
			std::wstring buffer(32768, '\0');

			swprintf_s(buffer.data(), buffer.size(),
				L"An exception has occurred on startup: %hs.\n\nFailed to initialize God of War mod. Note that the February 4 2022 patch"
				" (v1.0.443) is the minimum required version.\n\nExecutable path: %ws", e.what(), fullModulePath.c_str());

			MessageBoxW(nullptr, buffer.data(), L"Error", MB_ICONERROR);
		}
	}

	return TRUE;
}
