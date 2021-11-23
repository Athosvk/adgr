#include "./core/math/float2.h"
#include "./core/math/float3.h"
#include "./core/math/float4.h"

#include <sstream>

namespace CRT
{
	float2::float2()
		: x(0.0f)
		, y(0.0f)
	{ }

	float2::float2(float _s)
		: x(_s)
		, y(_s)
	{ }

	float2::float2(float _x, float _y)
		: x(_x)
		, y(_y)
	{ }

	float2::float2(const float3& _v)
		: x(_v.x)
		, y(_v.y)
	{ }

	float2::float2(const float4& _v)
		: x(_v.x)
		, y(_v.y)
	{ }

	float2 float2::Up()
	{
		return float2(0.0f, 1.0f);
	}

	float2 float2::Down()
	{
		return float2(0.0f, -1.0f);
	}

	float2 float2::Left()
	{
		return float2(-1.0f, 0.0f);
	}

	float2 float2::Right()
	{
		return float2(1.0f, 0.0f);
	}

	float2 float2::Zero()
	{
		return float2(0.0f, 0.0f);
	}

	float2 float2::One()
	{
		return float2(1.0f, 1.0f);
	}

	float2 float2::XAxis()
	{
		return float2(1.0f, 0.0f);
	}

	float2 float2::YAxis()
	{
		return float2(0.0f, 1.0f);
	}

	float2& float2::Add(const float2& _o)
	{
		x += _o.x;
		y += _o.y;

		return *this;
	}

	float2& float2::Subtract(const float2& _o)
	{
		x -= _o.x;
		y -= _o.y;

		return *this;
	}

	float2& float2::Multiply(const float2& _o)
	{
		x *= _o.x;
		y *= _o.y;

		return *this;
	}

	float2& float2::Divide(const float2& _o)
	{
		x /= _o.x;
		y /= _o.y;

		return *this;
	}

	float2& float2::Add(float _f)
	{
		x += _f;
		y += _f;

		return *this;
	}

	float2& float2::Subtract(float _f)
	{
		x -= _f;
		y -= _f;

		return *this;
	}

	float2& float2::Multiply(float _f)
	{
		x *= _f;
		y *= _f;

		return *this;
	}

	float2& float2::Divide(float _f)
	{
		x /= _f;
		y /= _f;

		return *this;
	}

	float2 operator+(float2 _l, const float2& _r)
	{
		return _l.Add(_r);
	}

	float2 operator-(float2 _l, const float2& _r)
	{
		return _l.Subtract(_r);
	}

	float2 operator*(float2 _l, const float2& _r)
	{
		return _l.Multiply(_r);
	}

	float2 operator/(float2 _l, const float2& _r)
	{
		return _l.Divide(_r);
	}

	float2 operator+(float2 _l, float _f)
	{
		return _l.Add(_f);
	}

	float2 operator-(float2 _l, float _f)
	{
		return _l.Subtract(_f);
	}

	float2 operator*(float2 _l, float _f)
	{
		return _l.Multiply(_f);
	}

	float2 operator/(float2 _l, float _f)
	{
		return _l.Divide(_f);
	}

	float2 operator-(const float2& _v)
	{
		return float2(-_v.x, -_v.y);
	}

	bool float2::operator==(const float2& _o) const
	{
		return (x == _o.x && y == _o.y);
	}

	bool float2::operator!=(const float2& _o) const
	{
		return !(*this == _o);
	}

	float2& float2::operator+=(const float2& _o)
	{
		return Add(_o);
	}

	float2& float2::operator-=(const float2& _o)
	{
		return Subtract(_o);
	}

	float2& float2::operator*=(const float2& _o)
	{
		return Multiply(_o);
	}

	float2& float2::operator/=(const float2& _o)
	{
		return Divide(_o);
	}

	float2& float2::operator+=(float _f)
	{
		return Add(_f);
	}

	float2& float2::operator-=(float _f)
	{
		return Subtract(_f);
	}

	float2& float2::operator*=(float _f)
	{
		return Multiply(_f);
	}

	float2& float2::operator/=(float _f)
	{
		return Divide(_f);
	}

	bool float2::operator<(const float2& _o) const
	{
		return (x < _o.x && y < _o.y);
	}

	bool float2::operator<=(const float2& _o) const
	{
		return (x <= _o.x && y <= _o.y);
	}

	bool float2::operator>(const float2& _o) const
	{
		return (x > _o.x && y > _o.y);
	}

	bool float2::operator>=(const float2& _o) const
	{
		return (x >= _o.x && y >= _o.y);
	}

	float2 float2::Normalise() const
	{
		float length = Magnitude();
		return float2(x / length, y / length);
	}

	float float2::Dot(const float2& _o) const
	{
		return (x * _o.x + y * _o.y);
	}

	float float2::Magnitude() const
	{
		return sqrt(x * x + y * y);
	}

	float float2::Distance(const float2 & _o) const
	{
		float a = x - _o.x;
		float b = y - _o.y;
		return sqrt(a * a + b * b);
	}
	std::string float2::ToString() const
	{
		std::stringstream result;
		result << "float2(" << x << ", " << y << ")";
		return result.str();
	}
}