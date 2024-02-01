#pragma once

namespace Kinetica
{

class EffectParm
{
public:
	char _pad0[0x40];
};
static_assert(sizeof(EffectParm) == 0x40);

}