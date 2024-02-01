#include <algorithm>
#include <exception>

#include "RTTICSharpExporter.h"

using namespace Kinetica;

RTTICSharpExporter::RTTICSharpExporter(const std::span<const RTTI::TypeDecl>& Types, const std::span<const RTTI::EnumDecl>& Enums)
{
	for (auto& e : Types)
		m_TypeDecls.push_back(&e);

	for (auto& e : Enums)
		m_EnumDecls.push_back(&e);

	// Sort both by type name
	std::sort(m_TypeDecls.begin(), m_TypeDecls.end(), [](const auto *A, const auto *B)
	{
		return A->GetTypeString() < B->GetTypeString();
	});

	std::sort(m_EnumDecls.begin(), m_EnumDecls.end(), [](const auto *A, const auto *B)
	{
		int result = A->GetTypeString().compare(B->GetTypeString());

		if (result == 0)
			return A->GetBinaryTypeId() < B->GetBinaryTypeId();

		return result < 0;
	});

	// Determine which enum names are duplicated
	for (auto type : m_EnumDecls)
		m_DuplicateEnumCounters[type->GetTypeString()]++;

	// Determine enum sizes by iterating over all class members
	for (auto type : m_TypeDecls)
	{
		auto members = type->CalculateMemberHeirarchy(false, true);

		for (auto member : members)
		{
			if (auto enumType = member->GetEnumType())
			{
				auto [itr, inserted] = m_CalculatedEnumSizes.try_emplace(enumType->GetBinaryTypeId(), member->GetTypeSize());

				if (!inserted)
				{
					if (itr->second != member->GetTypeSize())
						throw std::logic_error("Found an enumeration with an inconsistent size between different classes");
				}
			}
		}
	}
}

void RTTICSharpExporter::ExportAll(const std::string_view Directory)
{
	std::filesystem::create_directories(Directory);

	auto filePath = std::format("{0:}\\AllStructs.cs", Directory);

	if (fopen_s(&m_FileHandle, filePath.c_str(), "w") == 0)
	{
		ExportFileHeader();

		for (auto type : m_TypeDecls)
			ExportRTTIClass(type);

		ExportFileFooter();
		fclose(m_FileHandle);
	}

	filePath = std::format("{0:}\\AllEnums.cs", Directory);

	if (fopen_s(&m_FileHandle, filePath.c_str(), "w") == 0)
	{
		ExportFileHeader();

		for (auto type : m_EnumDecls)
			ExportRTTIEnum(type);

		ExportFileFooter();
		fclose(m_FileHandle);
	}
}

void RTTICSharpExporter::ExportFileHeader()
{
	const char *data =
		"\n"
		"using int8_t = System.SByte;\n"
		"using uint8_t = System.Byte;\n"
		"using int16_t = System.Int16;\n"
		"using uint16_t = System.UInt16;\n"
		"using int32_t = System.Int32;\n"
		"using uint32_t = System.UInt32;\n"
		"using int64_t = System.Int64;\n"
		"using uint64_t = System.UInt64;\n"
		"using half = System.Half;\n"
		"\n";

	Print("namespace GoWExtractorUtil.Kinetica.RTTI.Types;\n\n");
	Print("#pragma warning disable CS0649 // warning CS0649: 'member' is never assigned to, and will always have its default value 'value'.\n");
	Print("#pragma warning disable CS0108 // warning CS0108: 'class' hides inherited member 'member'. Use the new keyword if hiding was intended.\n");
	Print("#pragma warning disable CS8618 // warning CS8618: Non-nullable field must contain a non-null value when exiting constructor. Consider declaring as nullable.\n");
	Print(data);
}

void RTTICSharpExporter::ExportFileFooter()
{
}

void RTTICSharpExporter::ExportRTTIClass(const RTTI::TypeDecl *Type)
{
	auto classTypeString = Type->GetTypeString();
	auto inheritedType = Type->GetInheritedType();

	// Attributes/decl
	Print("[ClassSerializable(0x{0:X}, 0x{1:X})]\n", Type->GetBinaryTypeId(), Type->m_Size);

	if (inheritedType)
		Print("public class {0:} : {1:}\n{{\n", classTypeString, inheritedType->GetTypeString());
	else
		Print("public class {0:}\n{{\n", classTypeString);

	// Members
	auto members = Type->CalculateMemberHeirarchy();

	for (auto member : members)
	{
		auto memberTypeString = member->GetTypeString();

		Print("    [RTTIMember(0x{0:X}, 0x{1:X})] ", member->m_Offset, member->GetTypeSize());

		//
		// A few ugly hacks are needed for valid C# code generation:
		//
		// 1. Void is not a type. Replace it with object.
		// 2. Support for constants in generic parameters is not a thing. Replace FixedArray<T, #> with a custom type shim.
		// 3. Member names are not allowed to match the class name. Add a prefix.
		//
		// 1 & 2.
		if (size_t index = memberTypeString.find("<void>"); index != std::string::npos)
			memberTypeString = memberTypeString.replace(index, 6, "<object>");
		else if (memberTypeString.contains("FixedArray<"))
			memberTypeString = memberTypeString.replace(memberTypeString.find(", "), 2, ", FixedSize");

		if (auto enumType = member->GetEnumType(); enumType && m_DuplicateEnumCounters[enumType->GetTypeString()] > 1)
			Print("public {0:}_ID{1:} {2:}", memberTypeString, enumType->GetBinaryTypeId(), member->m_Name);
		else
			Print("public {0:} {1:}", memberTypeString, member->m_Name);

		// 3.
		if (classTypeString.compare(member->m_Name) == 0)
			Print("_");

		Print(";\n");
	}

	Print("}}\n\n");
}

void RTTICSharpExporter::ExportRTTIEnum(const RTTI::EnumDecl *Type)
{
	// Calculate the largest constant to determine if an explicit type is required
	uint64_t largestValue = 0;

	for (uint32_t i = 0; i < Type->m_EntryCount; i++)
		largestValue = std::max(Type->m_Values[i], largestValue);

	// Attributes/decl
	auto typeString = Type->GetTypeString();

	Print("[EnumSerializable(0x{0:X})]\n", Type->GetBinaryTypeId());

	if (m_DuplicateEnumCounters[typeString] > 1)
		Print("public enum {0:}_ID{1:}", typeString, Type->GetBinaryTypeId());
	else
		Print("public enum {0:}", typeString);

	uint64_t enumTypeSize = m_CalculatedEnumSizes[Type->GetBinaryTypeId()];

	switch (enumTypeSize)
	{
	case 1: Print(" : uint8_t"); break;
	case 2: Print(" : uint16_t"); break;
	case 4: Print(" : uint32_t"); break;
	case 8: Print(" : uint64_t"); break;

	default:
		if (largestValue > std::numeric_limits<uint32_t>::max())
		{
			Print(" : uint64_t");
			enumTypeSize = 8;
		}
		else
		{
			Print(" : uint32_t");
			enumTypeSize = 4;
		}
		break;
	}

	Print("\n{{\n");

	// Members
	for (uint32_t i = 0; i < Type->m_EntryCount; i++)
		Print("    {0:} = {1:},\n", Type->m_Entries[i], Type->m_Values[i] & ((1ull << (enumTypeSize * 8)) - 1));

	Print("}}\n\n");
}
