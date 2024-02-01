#include <algorithm>
#include <format>

#include "RTTI.h"

namespace Kinetica::RTTI
{

Lazy<HashMapAttribute *> HashMapAttributeTable([]()
{
	// Unbounded
	return Offsets::ResolveID<"RTTI::HashMapAttributeTable", HashMapAttribute *>();
});

Lazy<ArrayAttribute *> ArrayAttributeTable([]()
{
	// Unbounded
	return Offsets::ResolveID<"RTTI::ArrayAttributeTable", ArrayAttribute *>();
});

Lazy<TypeAttribute *> TypeAttributeTable([]()
{
	// Unbounded
	return Offsets::ResolveID<"RTTI::TypeAttributeTable", TypeAttribute *>();
});

Lazy<std::span<const TypeDecl>> AllTypeDecls([]()
{
	// (0x1411DF730 - 0x1411C80D0) / 0x50
	const auto allTypeDeclsPtr = Offsets::ResolveID<"RTTI::TypeDeclTable", TypeDecl *>();

	return std::span{ &allTypeDeclsPtr[0], &allTypeDeclsPtr[1198] };
});

Lazy<std::span<const EnumDecl>> AllEnumDecls([]()
{
	// (0x141174F20 - 0x1411712E0) / 0x20
	const auto allEnumsPtr = Offsets::ResolveID<"RTTI::EnumDeclTable", EnumDecl *>();

	return std::span { &allEnumsPtr[0], &allEnumsPtr[482] };
});

std::string_view TypeDecl::GetTypeString() const
{
	return m_Name;
}

uint32_t TypeDecl::GetBinaryTypeId() const
{
	return static_cast<uint32_t>(this - AllTypeDecls->data());
}

const TypeDecl *TypeDecl::GetInheritedType() const
{
	if (m_InheritedTypeDeclIndex == 0xFFFF)
		return nullptr;

	return &AllTypeDecls[m_InheritedTypeDeclIndex];
}

std::vector<const TypeAttribute *> TypeDecl::CalculateMemberHeirarchy(bool Sorted, bool IncludeAllInstanceMembers) const
{
	std::vector<const TypeAttribute *> sortedEntries;
	uint64_t thisParentBaseOffset = 0;

	if (GetInheritedType())
		thisParentBaseOffset = GetInheritedType()->m_Size;

	for (auto& member : Members())
	{
		auto attr = &TypeAttributeTable[member.m_TypeAttributeIndex];

		// By default, all members are listed in the game's table. That includes fields from instanced classes. If they
		// aren't desired, skip them.
		if (!IncludeAllInstanceMembers)
		{
			if (attr->GetParentType() != this)
				continue;

			// Parent type might not be enough. Check offsets too.
			if (attr->m_Offset < thisParentBaseOffset)
				continue;
		}

		sortedEntries.push_back(attr);
	}

	if (Sorted)
	{
		std::sort(sortedEntries.begin(), sortedEntries.end(), [](const TypeAttribute *A, const TypeAttribute *B)
		{
			if (A->m_Offset == B->m_Offset)
			{
				// Offsets can overlap. Class instances come first.
				if (A->m_Size == 0)
					return true;
				else if (B->m_Size == 0)
					return false;
			}

			return A->m_Offset < B->m_Offset;
		});
	}

	return sortedEntries;
}

std::string_view EnumDecl::GetTypeString() const
{
	return m_Name;
}

uint32_t EnumDecl::GetBinaryTypeId() const
{
	return static_cast<uint32_t>(this - AllEnumDecls->data());
}

TypeAttribute::Flag TypeAttribute::GetTypeFlag() const
{
	return static_cast<Flag>(m_TypeFlags & 3);
}

TypeAttribute::Type TypeAttribute::GetTypeId() const
{
	return static_cast<Type>(m_TypeFlags >> 2);
}

std::string TypeAttribute::GetTypeString() const
{
	switch (GetTypeId())
	{
	case Type::Primitive:
	{
		if (auto enumType = GetEnumType())
			return enumType->m_Name;

		switch (m_Size)
		{
		case 1:
			return "int8_t";
		case 2:
			return "int16_t";
		case 4:
			return "int32_t";
		case 8:
			return "int64_t";
		}
	}
	break;

	case Type::UnsignedPrimitive:
	{
		if (auto enumType = GetEnumType())
			return enumType->m_Name;

		switch (m_Size)
		{
		case 1:
			return "uint8_t";
		case 2:
			return "uint16_t";
		case 4:
			return "uint32_t";
		case 8:
			return "uint64_t";
		}
	}
	break;

	case Type::FloatingPoint:
	{
		switch (m_Size)
		{
		case 2:
			return "half";
		case 4:
			return "float";
		}
	}
	break;

	case Type::CharArray:
	{
		return "char[]";
	}
	break;

	case Type::StringHash:
	{
		return "StringHash";
	}
	break;

	case Type::Flags:
	{
		switch (m_Size)
		{
		case 1:
			return "bool";
		case 2:
		case 4:
		case 8:
			// These don't exist in the game RTTI? Not sure what to call them.
			break;
		}
	}
	break;

	case Type::CharArrayPointer:
	{
		return "CharArrayPtr";
	}
	break;

	case Type::Pointer:
	{
		if (m_CustomIndex == 0xFFFF)
			return "CPtr<void>";

		return std::format("CPtr<{0:}>", AllTypeDecls[m_CustomIndex].m_Name);
	}
	break;

	case Type::Reference:
	{
		return std::format("CRef<{0:}>", AllTypeDecls[m_CustomIndex].m_Name);
	}
	break;

	case Type::Array:
	{
		auto& arrayAttr = ArrayAttributeTable[m_CustomIndex];
		std::string_view arrayType;
		std::string containedType;

		if (arrayAttr.m_IsClassInstance)
			containedType = AllTypeDecls[arrayAttr.m_CustomIndex].m_Name;
		else
			containedType = TypeAttributeTable[arrayAttr.m_CustomIndex].GetTypeString();

		if (arrayAttr.IsFixedSize())
			return std::format("FixedArray<{0:}, {1:}>", containedType, arrayAttr.m_ElementCount);

		return std::format("Array<{0:}>", containedType);
	}
	break;

	case Type::HashMap:
	{
		auto& hashMapAttr = HashMapAttributeTable[m_CustomIndex];

		return std::format("HashMap<{0:}, {1:}>",
			TypeAttributeTable[hashMapAttr.m_KeyTypeAttributeIndex].GetTypeString(),
			TypeAttributeTable[hashMapAttr.m_ValueTypeAttributeIndex].GetTypeString());
	}
	break;

	case Type::ClassInstance:
	{
		return AllTypeDecls[m_CustomIndex].m_Name;
	}
	break;
	}

	return "<BAD_TYPE>";
}

uint64_t TypeAttribute::GetTypeSize() const
{
	if (GetTypeId() == Type::HashMap)
		return 0xC;

	if (GetTypeId() == Type::ClassInstance)
		return AllTypeDecls[m_CustomIndex].m_Size;

	return m_Size;
}

const TypeDecl *TypeAttribute::GetParentType() const
{
	// Honestly no idea what's being checked
	if (m_ParentTypeDeclIndex == m_Unknown18)
		return nullptr;

	return &AllTypeDecls[m_ParentTypeDeclIndex];
}

const EnumDecl *TypeAttribute::GetEnumType() const
{
	if (m_EnumDeclIndex == 0xFFFF)
		return nullptr;

	return &AllEnumDecls[m_EnumDeclIndex];
}

bool ArrayAttribute::IsFixedSize() const
{
	return m_ElementCount != 0;
}

}