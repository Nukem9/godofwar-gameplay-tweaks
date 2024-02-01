#pragma once

#include "Matrix.h"

namespace Kinetica
{

class goSoldier
{
public:
	virtual ~goSoldier();

	char _pad0[0x18];
	Matrix m_Matrix;
};

}