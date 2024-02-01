#pragma once

#include <math.h>

#include "Vector3.h"
#include "Vector4.h"

namespace Kinetica
{

class Matrix
{
public:
	Vector4F Rows[4];

	Matrix()
	{
		Rows[0].X = 1.0f;
		Rows[1].Y = 1.0f;
		Rows[2].Z = 1.0f;
		Rows[3].W = 1.0f;
	}

	Matrix(float Yaw, float Pitch, float Roll) : Matrix()
	{
		// From Euler angles
		const float cosY = cosf(Yaw);
		const float sinY = sinf(Yaw);
		const float cosP = cosf(Pitch);
		const float sinP = sinf(Pitch);
		const float cosR = cosf(Roll);
		const float sinR = sinf(Roll);

		Rows[0].X = cosY * cosR + sinY * sinP * sinR;
		Rows[1].X = cosR * sinY * sinP - sinR * cosY;
		Rows[2].X = cosP * sinY;

		Rows[0].Y = cosP * sinR;
		Rows[1].Y = cosR * cosP;
		Rows[2].Y = -sinP;

		Rows[0].Z = sinR * cosY * sinP - sinY * cosR;
		Rows[1].Z = sinY * sinR + cosR * cosY * sinP;
		Rows[2].Z = cosP * cosY;
	}

	Vector3F Right() const
	{
		return Vector3F(Rows[0].X, Rows[0].Y, Rows[0].Z);
	}

	Vector3F Up() const
	{
		return Vector3F(Rows[1].X, Rows[1].Y, Rows[1].Z);
	}

	Vector3F Forward() const
	{
		return -Vector3F(Rows[2].X, Rows[2].Y, Rows[2].Z);
	}

	Vector3F Position() const
	{
		return Vector3F(Rows[3].X, Rows[3].Y, Rows[3].Z);
	}
};
static_assert(sizeof(Matrix) == 0x40);

}