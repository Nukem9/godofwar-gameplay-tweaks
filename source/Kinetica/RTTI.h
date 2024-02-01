#pragma once

namespace Kinetica::RTTI
{

struct TypeDecl;
struct EnumDecl;
struct TypeAttribute;
struct ArrayAttribute;
struct HashMapAttribute;

extern Lazy<std::span<const TypeDecl>> AllTypeDecls;
extern Lazy<std::span<const EnumDecl>> AllEnumDecls;

struct TypeDecl final
{
	struct MemberEntry
	{
		uint64_t m_NameHashOrIndex;		// 0x0 Converted from a hash to a string pool index after initialization
		uint32_t m_TypeAttributeIndex;	// 0x8 TypeAttribute
	};
	static_assert(sizeof(MemberEntry) == 0x10);

	const char *m_TypeName;				// 0x0
	const char *m_Name;					// 0x8
	uint64_t m_Size;					// 0x10
	char _pad18[0x8];					// 0x18 Probably alignment and other flags
	uint32_t m_BaseTypeAttributeIndex;	// 0x20 Points to the first TypeAttribute index that corresponds to this class
	uint32_t m_MemberCount;				// 0x24
	uint16_t m_InheritedTypeDeclIndex;	// 0x28 TypeDecl
	int m_Unknown2C;					// 0x2C Usually -1
	void *m_TypeHash1;					// 0x30 Structure where the first element is the TypeName hash. Can be null.
	void *m_TypeHash2;					// 0x38 Identical to TypeHash1? Can be null.
	MemberEntry *m_Members;				// 0x40 Sorted by string hash?
	char _pad48[0x8];					// 0x48

	auto Members() const
	{
		return std::span{ m_Members, m_MemberCount };
	}

	std::string_view GetTypeString() const;
	uint32_t GetBinaryTypeId() const;
	const TypeDecl *GetInheritedType() const;
	std::vector<const TypeAttribute *> CalculateMemberHeirarchy(bool Sorted = true, bool IncludeAllInstanceMembers = false) const;
};
static_assert(sizeof(TypeDecl) == 0x50);

struct EnumDecl final
{
	const char *m_Name;		// 0x0
	int m_Unknown;			// 0x8 Points to an index in another table but I don't know what it's doing (sub_140CF2E50)
	uint32_t m_EntryCount;	// 0xC
	const char **m_Entries;	// 0x10
	uint64_t *m_Values;		// 0x18

	std::string_view GetTypeString() const;
	uint32_t GetBinaryTypeId() const;
};
static_assert(sizeof(EnumDecl) == 0x20);

struct TypeAttribute final
{
	enum class Type : uint8_t
	{
		Primitive = 0,			// (xxx)
		UnsignedPrimitive = 1,	// (xxx)
		FloatingPoint = 2,		// (xxx) Float or half depending on the size
		CharArray = 3,			// (xxx) Strcpy into buffer at offset. Doesn't exist in RTTI?
		StringHash = 4,			// (u64)
		Flags = 5,				// (xxx) Boolean that is 1-8 bytes? Most don't exist in RTTI?
		CharArrayPointer = 6,	// (u64) Allocated on demand
		Pointer = 7,			// (u64)
		Reference = 8,			// (u64) Guessed. Haven't found this used in the code yet.
		Array = 9,				// (xxx)
		HashMap = 10,			// (xxx) See 140CF27A0 to determine the number of elements
		ClassInstance = 11,		// (xxx) Instance member
	};

	enum class Flag : uint8_t
	{
		None = 0,		// Default
		Unknown1 = 1,	// Read only?
		Unknown2 = 2,	// Read only? Appears mostly on IsNull variables.
	};

	const char *m_Name;				// 0x0
	const char *m_TypeName;			// 0x8
	uint16_t m_Offset;				// 0x10
	uint16_t m_Size;				// 0x12
	uint8_t m_TypeFlags;			// 0x14
	uint8_t m_Unknown15;			// 0x15 if == 1, memcpy values without any conversion or marshaling? Bypasses m_TypeFlags checks.
	uint16_t m_ParentTypeDeclIndex;	// 0x16 TypeDecl
	uint16_t m_Unknown18;			// 0x18 Usually -1
	uint16_t m_CustomIndex;			// 0x1A Points into multiple different tables depending on the type
	uint16_t m_EnumDeclIndex;		// 0x1C EnumDecl
	uint16_t m_Unknown1E;			// 0x1E Usually -1

	std::string GetTypeString() const;
	uint64_t GetTypeSize() const;
	Flag GetTypeFlag() const;
	Type GetTypeId() const;
	const TypeDecl *GetParentType() const;
	const EnumDecl *GetEnumType() const;
};
static_assert(sizeof(TypeAttribute) == 0x20);

struct ArrayAttribute final
{
	uint32_t m_ElementCount;	// 0x0
	bool m_IsClassInstance;		// 0x4
	uint32_t m_CustomIndex;		// 0x8 TypeAttributeIndex or TypeDeclIndex based on m_IsClassInstance
	char _padC[0xC];			// 0xC

	bool IsFixedSize() const;
};
static_assert(sizeof(ArrayAttribute) == 0x18);

struct HashMapAttribute final
{
	uint32_t m_KeyTypeAttributeIndex;	// 0x0
	uint32_t m_ValueTypeAttributeIndex;	// 0x4
	uint64_t m_EntrySize;				// 0x8
	uint64_t m_ValueSize;				// 0x10
};
static_assert(sizeof(HashMapAttribute) == 0x18);

}