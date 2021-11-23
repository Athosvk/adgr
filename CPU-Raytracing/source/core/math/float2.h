#pragma once
#include <string>

namespace CRT
{
	class float3;
	class float4;

	class float2
	{
	public:
		float2();
		float2(float _s);
		float2(float _x, float _y);
		float2(const float3& _v);
		float2(const float4& _v);

		static float2 Up();
		static float2 Down();
		static float2 Left();
		static float2 Right();

		static float2 Zero();
		static float2 One();

		static float2 XAxis();
		static float2 YAxis();

		float2& Add(const float2& _o);
		float2& Subtract(const float2& _o);
		float2& Multiply(const float2& _o);
		float2& Divide(const float2& _o);

		float2& Add(float _f);
		float2& Subtract(float _f);
		float2& Multiply(float _f);
		float2& Divide(float _f);

		friend float2 operator+(float2 _l, const float2& _r);
		friend float2 operator-(float2 _l, const float2& _r);
		friend float2 operator*(float2 _l, const float2& _r);
		friend float2 operator/(float2 _l, const float2& _r);

		friend float2 operator+(float2 _l, float _f);
		friend float2 operator-(float2 _l, float _f);
		friend float2 operator*(float2 _l, float _f);
		friend float2 operator/(float2 _l, float _f);

		friend float2 operator-(const float2& _v);

		bool operator==(const float2& _o) const;
		bool operator!=(const float2& _o) const;

		float2& operator+=(const float2& _o);
		float2& operator-=(const float2& _o);
		float2& operator*=(const float2& _o);
		float2& operator/=(const float2& _o);

		float2& operator+=(float _f);
		float2& operator-=(float _f);
		float2& operator*=(float _f);
		float2& operator/=(float _f);

		bool operator< (const float2& _o) const;
		bool operator<=(const float2& _o) const;
		bool operator> (const float2& _o) const;
		bool operator>=(const float2& _o) const;

		float2 Normalise() const;
		float  Dot(const float2& _o) const;

		float Magnitude() const;
		float Distance(const float2& _o) const;

		std::string ToString() const;

		// Data
		union
		{
			float f[2];
			struct { float x, y; };
		};
	};
}