#include "./core/math/float2.h"
#include "./core/math/float3.h"
#include "./core/math/float4.h"

#include <sstream>

namespace CRT
{
	float4::float4()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
		, w(0.0f)
	{ }

	float4::float4(float _s)
		: x(_s)
		, y(_s)
		, z(_s)
		, w(_s)
	{ }

	float4::float4(float _x, float _y, float _z, float _w)
		: x(_x)
		, y(_y)
		, z(_z)
		, w(_w)
	{ }

	float4::float4(const float2& _v)
		: x(_v.x)
		, y(_v.y)
		, z(0.0f)
		, w(0.0f)
	{ }

	float4::float4(const float3& _v)
		: x(_v.x)
		, y(_v.y)
		, z(_v.z)
		, w(0.0f)
	{ }

	float4 float4::Zero()
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	float4 float4::One()
	{
		return float4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	float4& float4::Add(const float4& _o)
	{
		x += _o.x;
		y += _o.y;
		z += _o.z;
		w += _o.w;

		return *this;
	}

	float4& float4::Subtract(const float4& _o)
	{
		x -= _o.x;
		y -= _o.y;
		z -= _o.z;
		w -= _o.w;

		return *this;
	}

	float4& float4::Multiply(const float4& _o)
	{
		x *= _o.x;
		y *= _o.y;
		z *= _o.z;
		w *= _o.w;

		return *this;
	}

	float4& float4::Divide(const float4& _o)
	{
		x /= _o.x;
		y /= _o.y;
		z /= _o.z;
		w /= _o.w;

		return *this;
	}

	float4& float4::Add(float _f)
	{
		x += _f;
		y += _f;
		z += _f;
		w += _f;

		return *this;
	}

	float4& float4::Subtract(float _f)
	{
		x -= _f;
		y -= _f;
		z -= _f;
		w -= _f;

		return *this;
	}

	float4& float4::Multiply(float _f)
	{
		x *= _f;
		y *= _f;
		z *= _f;
		w *= _f;

		return *this;
	}

	float4& float4::Divide(float _f)
	{
		x /= _f;
		y /= _f;
		z /= _f;
		w /= _f;

		return *this;
	}

	float4 operator+(float4 _l, const float4& _r)
	{
		return _l.Add(_r);
	}

	float4 operator-(float4 _l, const float4& _r)
	{
		return _l.Subtract(_r);
	}

	float4 operator*(float4 _l, const float4& _r)
	{
		return _l.Multiply(_r);
	}

	float4 operator/(float4 _l, const float4& _r)
	{
		return _l.Divide(_r);
	}

	float4 operator+(float4 _l, float _f)
	{
		return _l.Add(_f);
	}

	float4 operator-(float4 _l, float _f)
	{
		return _l.Subtract(_f);
	}

	float4 operator*(float4 _l, float _f)
	{
		return _l.Multiply(_f);
	}

	float4 operator/(float4 _l, float _f)
	{
		return _l.Divide(_f);
	}

	float4 operator-(const float4& _v)
	{
		return float4(-_v.x, -_v.y, -_v.z, -_v.w);
	}

	bool float4::operator==(const float4& _o) const
	{
		return (x == _o.x && y == _o.y && z == _o.z && w == _o.w);
	}

	bool float4::operator!=(const float4& _o) const
	{
		return !(*this == _o);
	}

	float4& float4::operator+=(const float4& _o)
	{
		return Add(_o);
	}

	float4& float4::operator-=(const float4& _o)
	{
		return Subtract(_o);
	}

	float4& float4::operator*=(const float4& _o)
	{
		return Multiply(_o);
	}

	float4& float4::operator/=(const float4& _o)
	{
		return Divide(_o);
	}

	float4& float4::operator+=(float _o)
	{
		return Add(_o);
	}

	float4& float4::operator-=(float _o)
	{
		return Subtract(_o);
	}

	float4& float4::operator*=(float _o)
	{
		return Multiply(_o);
	}

	float4& float4::operator/=(float _o)
	{
		return Divide(_o);
	}

	bool float4::operator<(const float4& _o) const
	{
		return (x < _o.x && y < _o.y && z < _o.z && z < _o.z);
	}

	bool float4::operator<=(const float4& _o) const
	{
		return (x <= _o.x && y <= _o.y && z <= _o.z && z <= _o.z);
	}

	bool float4::operator>(const float4& _o) const
	{
		return (x > _o.x && y > _o.y && z > _o.z && z > _o.z);
	}

	bool float4::operator>=(const float4& _o) const
	{
		return (x >= _o.x && y >= _o.y && z >= _o.z && z >= _o.z);
	}

	float float4::Dot(const float4& _o) const
	{
		return x * _o.x + y * _o.y + z * _o.z + w * _o.w;
	}

	std::string float4::ToString() const
	{
		std::stringstream result;
		result << "float4(" << x << ", " << y << ", " << z << ", " << w << ")";
		return result.str();
	}
}