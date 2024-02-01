#include <stdexcept>
#include <Windows.h>
#include <detours/detours.h>

#include "Offsets.h"
#include "XUtil.h"

struct IATEnumContext
{
	const char *ModuleName = nullptr;
	std::variant<const char *, int> ImportName;
	const void *CallbackFunction = nullptr;
	void **OriginalFunction = nullptr;
	bool ModuleFound = false;
	bool Succeeded = false;
};

namespace Offsets
{

std::unordered_map<uint64_t, uintptr_t> OffsetMapping;

std::pair<uintptr_t, uintptr_t> GetModule()
{
	const static uintptr_t moduleBase = reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
	const static uintptr_t moduleEnd = [&]()
	{
		auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS64>(moduleBase + reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBase)->e_lfanew);
		return moduleBase + ntHeaders->OptionalHeader.SizeOfImage;
	}();

	return { moduleBase, moduleEnd };
}

std::pair<uintptr_t, uintptr_t> GetModuleSection(const std::string_view Section)
{
	uintptr_t base = 0;
	uintptr_t end = 0;

	if (!XUtil::GetPESectionRange(GetModule().first, Section.data(), &base, &end))
		__debugbreak();

	return { base, end };
}

std::pair<uintptr_t, uintptr_t> GetCodeSection()
{
	const static auto section = GetModuleSection(".text");
	return section;
}

std::pair<uintptr_t, uintptr_t> GetRdataSection()
{
	const static auto section = GetModuleSection(".rdata");
	return section;
}

std::pair<uintptr_t, uintptr_t> GetDataSection()
{
	const static auto section = GetModuleSection(".data");
	return section;
}

uintptr_t WriteJump(uintptr_t Address, uintptr_t Destination)
{
	DetourSetIgnoreTooSmall(true);

	if (DetourTransactionBegin() != NO_ERROR)
		return 0;

	auto originalFunction = reinterpret_cast<void **>(&Address);
	auto destinationFunction = reinterpret_cast<void *>(Destination);

	DetourUpdateThread(GetCurrentThread());
	DetourAttach(originalFunction, destinationFunction);

	if (DetourTransactionCommit() != NO_ERROR)
		return 0;

	return Address;
}

uintptr_t WriteCall(uintptr_t Address, uintptr_t Destination)
{
	// Manually calculate instruction length since detours pads with INT3s. We
	// want NOPs instead.
	for (auto end = Address; (end - Address) < 5;)
	{
		end = reinterpret_cast<uintptr_t>(DetourCopyInstruction(nullptr, nullptr, reinterpret_cast<void *>(end), nullptr, nullptr));

		for (auto i = Address; i < end; i++)
			WriteMemory(i, { 0x90 });
	}

	const auto result = WriteJump(Address, Destination);

	if (result != 0)
	{
		// Swap relative jump to relative call opcode
		WriteMemory(Address, { 0xE8 });
	}

	return result;
}

bool RedirectImport(
	uintptr_t ModuleHandle,
	const char *ImportModuleName,
	std::variant<const char *, int> ImportName,
	const void *CallbackFunction,
	void **OriginalFunction)
{
	auto moduleCallback = [](PVOID Context, HMODULE, LPCSTR Name) -> BOOL
	{
		auto c = static_cast<IATEnumContext *>(Context);

		c->ModuleFound = Name && _stricmp(Name, c->ModuleName) == 0;
		return !c->Succeeded;
	};

	auto importCallback = [](PVOID Context, ULONG Ordinal, PCSTR Name, PVOID *Func) -> BOOL
	{
		auto c = static_cast<IATEnumContext *>(Context);

		if (!c->ModuleFound)
			return false;

		// If the import name matches...
		const bool matches = [&]()
		{
			if (!Func)
				return false;

			if (std::holds_alternative<const char *>(c->ImportName))
				return _stricmp(Name, std::get<const char *>(c->ImportName)) == 0;

			return std::cmp_equal(Ordinal, std::get<int>(c->ImportName));
		}();

		if (matches)
		{
			// ...swap out the IAT pointer
			if (c->OriginalFunction)
				*c->OriginalFunction = *Func;

			WriteMemory(
				reinterpret_cast<uintptr_t>(Func),
				reinterpret_cast<const uint8_t *>(&c->CallbackFunction),
				sizeof(void *));

			c->Succeeded = true;
			return false;
		}

		return true;
	};

	IATEnumContext context {
		.ModuleName = ImportModuleName,
		.ImportName = ImportName,
		.CallbackFunction = CallbackFunction,
		.OriginalFunction = OriginalFunction,
	};

	if (!ModuleHandle)
		ModuleHandle = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));

	DetourEnumerateImportsEx(reinterpret_cast<HMODULE>(ModuleHandle), &context, moduleCallback, importCallback);
	return context.Succeeded;
}

void WriteMemory(uintptr_t Address, const uint8_t *Data, size_t Size)
{
	auto asPointer = reinterpret_cast<void *>(Address);
	DWORD protect = 0;

	VirtualProtect(asPointer, Size, PAGE_EXECUTE_READWRITE, &protect);

	for (uintptr_t i = Address; i < (Address + Size); i++)
		*reinterpret_cast<uint8_t *>(i) = *Data++;

	VirtualProtect(asPointer, Size, protect, &protect);
	FlushInstructionCache(GetCurrentProcess(), asPointer, Size);
}

void WriteMemory(uintptr_t Address, const std::initializer_list<uint8_t> Data)
{
	WriteMemory(Address, Data.begin(), Data.size());
}

void MapAddress(const std::string_view ID, uintptr_t Offset)
{
	auto hash = detail::LiteralHash::FNV1A(ID);
	auto found = OffsetMapping.find(hash);

	if (Offset == 0)
		throw std::runtime_error("Trying to map an address to 0");

	if (found != OffsetMapping.end())
		throw std::runtime_error("Trying to map an address that was previously mapped");

	OffsetMapping.emplace(hash, Offset);
}

void MapSignature(const std::string_view ID, const std::string_view Signature, int Adjustment)
{
	auto [moduleBase, moduleEnd] = GetModule();
	uintptr_t address = XUtil::FindPattern(moduleBase, moduleEnd - moduleBase, Signature.data());

	if (address == 0)
		throw std::runtime_error("Failed to find signature");

	MapAddress(ID, address - moduleBase + Adjustment);
}

uintptr_t FindOffset(const std::string_view ID)
{
	return OffsetMapping.at(detail::LiteralHash::FNV1A(ID));
}

uintptr_t FindOffset(uint64_t IDHash)
{
	return OffsetMapping.at(IDHash);
}

}
