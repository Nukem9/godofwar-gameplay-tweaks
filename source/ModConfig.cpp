#include <toml++/toml.h>

#include "ModConfig.h"

namespace ModConfig
{

const std::string_view ConfigurationFileName = "loader_config.toml";
std::filesystem::path GameRootPath;
std::filesystem::path ModDirectoryPath;

void Initialize(const std::filesystem::path& RootPath)
{
	GameRootPath = RootPath;

	ModDirectoryPath = RootPath;
	ModDirectoryPath.append("mods");

	if (!detail::LoadFromFile())
		throw std::runtime_error(std::format("Unable to load {0:} from the mods directory. Permission denied or file doesn't exist", ConfigurationFileName));
}

const std::filesystem::path& GetGameDirectory()
{
	return GameRootPath;
}

const std::filesystem::path& GetModDirectory()
{
	return ModDirectoryPath;
}

std::filesystem::path GetModRelativePath(const std::string_view PartialPath)
{
	auto path = ModDirectoryPath;
	path.append(PartialPath);

	return path;
}

namespace detail
{

bool LoadFromFile()
{
	// Try to parse toml data from disk
	toml::table table;

	try
	{
		table = toml::parse_file(GetModRelativePath(ConfigurationFileName).string());
	}
	catch (const toml::parse_error&)
	{
		return false;
	}

#define PARSE_TOML_MEMBER(obj, x) obj::x = (*category)[#x].value_or(decltype(obj::x){})
#define PARSE_TOML_HOTKEY(obj, x) Hotkeys::x = (*category)[#x].value_or(-1)

	// [General]
	if (auto category = table["General"].as_table())
	{
		PARSE_TOML_MEMBER(General, EnableDebugMenu);
		PARSE_TOML_MEMBER(General, DebugMenuFontScale);
	}

	// [Gameplay]
	if (auto category = table["Gameplay"].as_table())
	{
		PARSE_TOML_MEMBER(Gameplay, SkipIntroLogos);
		PARSE_TOML_MEMBER(Gameplay, RemoveDepthOfField);
		PARSE_TOML_MEMBER(Gameplay, UnlockPhotomodeCamera);
		PARSE_TOML_MEMBER(Gameplay, OverrideCameraFov);
		PARSE_TOML_MEMBER(Gameplay, CameraFov);
	}

	// [Hotkeys]
	if (auto category = table["Hotkeys"].as_table())
	{
		PARSE_TOML_HOTKEY(Hotkeys, ToggleDebugUI);
		PARSE_TOML_HOTKEY(Hotkeys, ToggleFreeflyCamera);
		PARSE_TOML_HOTKEY(Hotkeys, ToggleNoclip);
	}

	// [Lua]
	if (auto category = table["Lua"].as_table())
	{
		PARSE_TOML_MEMBER(Lua, LoadScripts);
		PARSE_TOML_MEMBER(Lua, DumpScripts);
	}

	// [Advanced]
	if (auto category = table["Advanced"].as_table())
	{
		PARSE_TOML_MEMBER(Advanced, ConsoleLogLevel);
		PARSE_TOML_MEMBER(Advanced, FileLogLevel);
		PARSE_TOML_MEMBER(Advanced, RemoveWindowsVersionCheck);
	}

#undef PARSE_TOML_MEMBER
#undef PARSE_TOML_HOTKEY

	return true;
}

}

}