#pragma once

namespace Kinetica
{

class Position
{
public:
	float X;// 0x0
	float Y;// 0x4
	float Z;// 0x8
};
static_assert(sizeof(Position) == 0xC);

}