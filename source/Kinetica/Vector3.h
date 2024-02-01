#pragma once

namespace Kinetica
{

class Vector3F
{
public:
	float X = 0.0f;
	float Y = 0.0f;
	float Z = 0.0f;

	Vector3F CrossProduct(const Vector3F& Other) const
	{
		return Vector3F
		{
			.X = (this->Y * Other.Z) - (this->Z * Other.Y),
			.Y = (this->Z * Other.X) - (this->X * Other.Z),
			.Z = (this->X * Other.Y) - (this->Y * Other.X),
		};
	}

	Vector3F operator-() const
	{
		return Vector3F
		{
			.X = -X,
			.Y = -Y,
			.Z = -Z,
		};
	}

	Vector3F operator*(const Vector3F& Other) const
	{
		return Vector3F
		{
			.X = this->X * Other.X,
			.Y = this->Y * Other.Y,
			.Z = this->Z * Other.Z,
		};
	}

	Vector3F operator*(float Scale) const
	{
		return Vector3F
		{
			.X = this->X * Scale,
			.Y = this->Y * Scale,
			.Z = this->Z * Scale,
		};
	}

	Vector3F& operator+=(const Vector3F& Other)
	{
		X += Other.X;
		Y += Other.Y;
		Z += Other.Z;
		return *this;
	}

	Vector3F& operator-=(const Vector3F& Other)
	{
		X -= Other.X;
		Y -= Other.Y;
		Z -= Other.Z;
		return *this;
	}
};
static_assert(sizeof(Vector3F) == 0xC);

}