#pragma once

namespace ModConfig
{

// [General]
struct General
{
	inline static bool EnableDebugMenu;
	inline static float DebugMenuFontScale;
};

// [Gameplay]
struct Gameplay
{
	inline static bool SkipIntroLogos;
	inline static bool RemoveDepthOfField;
	inline static bool UnlockPhotomodeCamera;
	inline static bool OverrideCameraFov;
	inline static float CameraFov;
};

// [Hotkeys]
struct Hotkeys
{
	inline static int ToggleDebugUI;
	inline static int ToggleFreeflyCamera;
	inline static int ToggleNoclip;
};

// [Lua]
struct Lua
{
	inline static bool LoadScripts;
	inline static bool DumpScripts;
};

// [Advanced]
struct Advanced
{
	inline static std::string ConsoleLogLevel;
	inline static std::string FileLogLevel;
	inline static bool RemoveWindowsVersionCheck;
};

void Initialize(const std::filesystem::path& RootPath);
const std::filesystem::path& GetGameDirectory();
const std::filesystem::path& GetModDirectory();
std::filesystem::path GetModRelativePath(const std::string_view PartialPath);

namespace detail
{

bool LoadFromFile();

}

}