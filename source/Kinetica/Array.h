#pragma once

#pragma pack(push, 0x4)

template<typename T>
class Array
{
public:
	T *m_Data;
	uint32_t m_Size;
};

#pragma pack(pop)