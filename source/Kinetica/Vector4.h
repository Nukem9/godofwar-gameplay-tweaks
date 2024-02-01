#pragma once

namespace Kinetica
{

class Vector4F
{
public:
	float X = 0.0f;
	float Y = 0.0f;
	float Z = 0.0f;
	float W = 0.0f;

	Vector4F operator*(const Vector4F& Other) const
	{
		return Vector4F
		{
			.X = this->X * Other.X,
			.Y = this->Y * Other.Y,
			.Z = this->Z * Other.Z,
			.W = this->W * Other.W,
		};
	}

	Vector4F operator*(float Scale) const
	{
		return Vector4F
		{
			.X = this->X * Scale,
			.Y = this->Y * Scale,
			.Z = this->Z * Scale,
			.W = this->W * Scale,
		};
	}

	Vector4F& operator+=(const Vector4F& Other)
	{
		X += Other.X;
		Y += Other.Y;
		Z += Other.Z;
		W += Other.W;
		return *this;
	}

	Vector4F& operator-=(const Vector4F& Other)
	{
		X -= Other.X;
		Y -= Other.Y;
		Z -= Other.Z;
		W -= Other.W;
		return *this;
	}
};
static_assert(sizeof(Vector4F) == 0x10);

}