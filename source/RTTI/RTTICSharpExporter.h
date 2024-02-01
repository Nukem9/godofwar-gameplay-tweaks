#pragma once

#include <format>
#include <stdio.h>

#include <Kinetica/RTTI.h>

class RTTICSharpExporter
{
private:
	FILE *m_FileHandle = nullptr;
	std::vector<const Kinetica::RTTI::TypeDecl *> m_TypeDecls;
	std::vector<const Kinetica::RTTI::EnumDecl *> m_EnumDecls;
	std::unordered_map<std::string_view, int> m_DuplicateEnumCounters;
	std::unordered_map<uint32_t, uint64_t> m_CalculatedEnumSizes;

public:
	RTTICSharpExporter() = delete;
	RTTICSharpExporter(const RTTICSharpExporter&) = delete;
	RTTICSharpExporter(const std::span<const Kinetica::RTTI::TypeDecl>& Types, const std::span<const Kinetica::RTTI::EnumDecl>& Enums);
	RTTICSharpExporter& operator=(const RTTICSharpExporter&) = delete;

	void ExportAll(const std::string_view Directory);

private:
	void ExportFileHeader();
	void ExportFileFooter();
	void ExportRTTIClass(const Kinetica::RTTI::TypeDecl *Type);
	void ExportRTTIEnum(const Kinetica::RTTI::EnumDecl *Type);

	template<typename... TArgs>
	void Print(const std::string_view Format, TArgs&&... Args)
	{
		auto formatted = std::vformat(Format, std::make_format_args(Args...));
		fputs(formatted.c_str(), m_FileHandle);
	}
};