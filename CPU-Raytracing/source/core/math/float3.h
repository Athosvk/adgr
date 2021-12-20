#pragma once
#include <string>

namespace CRT
{
	class float2;
	class float4;

	class float3
	{
	public:
		float3();
		float3(float _s);
		float3(float _x, float _y, float _z);
		float3(const float2& _v);
		float3(const float2& _v, float _z);
		float3(const float4& _v);

		static float3 Up();
		static float3 Down();
		static float3 Left();
		static float3 Right();
		static float3 Forward();
		static float3 Back();

		static float3 Zero();
		static float3 One();

		static float3 XAxis();
		static float3 YAxis();
		static float3 ZAxis();

		static float3 ComponentMin(std::initializer_list<float3> values);
		static float3 ComponentMax(std::initializer_list<float3> values);

		float3& Add(const float3& _o);
		float3& Subtract(const float3& _o);
		float3& Multiply(const float3& _o);
		float3& Divide(const float3& _o);

		float3& Add(float _f);
		float3& Subtract(float _f);
		float3& Multiply(float _f);
		float3& Divide(float _f);

		friend float3 operator+(float3 _l, const float3& _r);
		friend float3 operator-(float3 _l, const float3& _r);
		friend float3 operator*(float3 _l, const float3& _r);
		friend float3 operator/(float3 _l, const float3& _r);

		friend float3 operator+(float3 _l, float _f);
		friend float3 operator-(float3 _l, float _f);
		friend float3 operator*(float3 _l, float _f);
		friend float3 operator/(float3 _l, float _f);

		friend float3 operator-(const float3& _v);

		bool operator==(const float3& _o) const;
		bool operator!=(const float3& _o) const;

		float3& operator+=(const float3& _o);
		float3& operator-=(const float3& _o);
		float3& operator*=(const float3& _o);
		float3& operator/=(const float3& _o);

		float3& operator+=(float _o);
		float3& operator-=(float _o);
		float3& operator*=(float _o);
		float3& operator/=(float _o);

		bool operator< (const float3& _o) const;
		bool operator<=(const float3& _o) const;
		bool operator> (const float3& _o) const;
		bool operator>=(const float3& _o) const;

		float3 Normalize() const;
		float3 Cross(const float3& _o) const;
		float  Dot(const float3& _o) const;

		float Magnitude() const;
		float Distance(const float3& _o) const;
		float DistanceSquared(const float3& _o) const;
		float MagnitudeSquared() const;

		float3 ComponentMin(float3 other) const;
		float3 ComponentMax(float3 other) const;

		std::string ToString() const;

		union
		{
			float f[3];
			struct { float x, y, z; };
		};
	};
}