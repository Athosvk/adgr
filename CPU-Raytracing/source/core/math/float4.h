#pragma once
#include <string>

namespace CRT
{
	class float2;
	class float3;

	class float4
	{
	public:
		float4();
		float4(float _s);
		float4(float _x, float _y, float _z, float _w);
		float4(const float2& _v);
		float4(const float3& _v);

		static float4 Zero();
		static float4 One();

		float4& Add(const float4& _o);
		float4& Subtract(const float4& _o);
		float4& Multiply(const float4& _o);
		float4& Divide(const float4& _o);

		float4& Add(float _f);
		float4& Subtract(float _f);
		float4& Multiply(float _f);
		float4& Divide(float _f);

		friend float4 operator+(float4 _l, const float4& _r);
		friend float4 operator-(float4 _l, const float4& _r);
		friend float4 operator*(float4 _l, const float4& _r);
		friend float4 operator/(float4 _l, const float4& _r);

		friend float4 operator+(float4 _l, float _f);
		friend float4 operator-(float4 _l, float _f);
		friend float4 operator*(float4 _l, float _f);
		friend float4 operator/(float4 _l, float _f);

		friend float4 operator-(const float4& _v);

		bool operator==(const float4& _o) const;
		bool operator!=(const float4& _o) const;

		float4& operator+=(const float4& _o);
		float4& operator-=(const float4& _o);
		float4& operator*=(const float4& _o);
		float4& operator/=(const float4& _o);

		float4& operator+=(float _o);
		float4& operator-=(float _o);
		float4& operator*=(float _o);
		float4& operator/=(float _o);

		bool operator< (const float4& _o) const;
		bool operator<=(const float4& _o) const;
		bool operator> (const float4& _o) const;
		bool operator>=(const float4& _o) const;

		float  Dot(const float4& _o) const;

		std::string ToString() const;

		union
		{
			float f[4];
			struct { float x, y, z, w; };
		};
	};
}