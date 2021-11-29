#include "./core/math/float2.h"
#include "./core/math/float3.h"
#include "./core/math/float4.h"

#include <sstream>

namespace CRT
{
	float3::float3()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
	{ }

	float3::float3(float _s)
		: x(_s)
		, y(_s)
		, z(_s)
	{ }

	float3::float3(float _x, float _y, float _z)
		: x(_x)
		, y(_y)
		, z(_z)
	{ }

	float3::float3(const float2& _v)
		: x(_v.x)
		, y(_v.y)
		, z(0.0f)
	{ }

	float3::float3(const float2& _v, float _z)
		: x(_v.x)
		, y(_v.y)
		, z(_z)
	{ }

	float3::float3(const float4& _v)
		: x(_v.x)
		, y(_v.y)
		, z(_v.z)
	{ }

	float3 float3::Up()
	{
		return float3(0.0f, 1.0f, 0.0f);
	}

	float3 float3::Down()
	{
		return float3(0.0f, -1.0f, 0.0f);
	}

	float3 float3::Left()
	{
		return float3(-1.0f, 0.0f, 0.0f);
	}

	float3 float3::Right()
	{
		return float3(1.0f, 0.0f, 0.0f);
	}

	float3 float3::Forward()
	{
		return float3(0.0f, 0.0f, -1.0f);
	}

	float3 float3::Back()
	{
		return float3(0.0f, 0.0f, 1.0f);
	}

	float3 float3::Zero()
	{
		return float3(0.0f, 0.0f, 0.0f);
	}

	float3 float3::One()
	{
		return float3(1.0f, 1.0f, 1.0f);
	}

	float3 float3::XAxis()
	{
		return float3(1.0f, 0.0f, 0.0f);
	}

	float3 float3::YAxis()
	{
		return float3(0.0f, 1.0f, 0.0f);
	}

	float3 float3::ZAxis()
	{
		return float3(0.0f, 0.0f, 1.0f);
	}

	float3& float3::Add(const float3& _o)
	{
		x += _o.x;
		y += _o.y;
		z += _o.z;

		return *this;
	}

	float3& float3::Subtract(const float3& _o)
	{
		x -= _o.x;
		y -= _o.y;
		z -= _o.z;

		return *this;
	}

	float3& float3::Multiply(const float3& _o)
	{
		x *= _o.x;
		y *= _o.y;
		z *= _o.z;

		return *this;
	}

	float3& float3::Divide(const float3& _o)
	{
		x /= _o.x;
		y /= _o.y;
		z /= _o.z;

		return *this;
	}

	float3& float3::Add(float _f)
	{
		x += _f;
		y += _f;
		z += _f;

		return *this;
	}

	float3& float3::Subtract(float _f)
	{
		x -= _f;
		y -= _f;
		z -= _f;

		return *this;
	}

	float3& float3::Multiply(float _f)
	{
		x *= _f;
		y *= _f;
		z *= _f;

		return *this;
	}

	float3& float3::Divide(float _f)
	{
		x /= _f;
		y /= _f;
		z /= _f;

		return *this;
	}

	float3 operator+(float3 _l, const float3& _r)
	{
		return _l.Add(_r);
	}

	float3 operator-(float3 _l, const float3& _r)
	{
		return _l.Subtract(_r);
	}

	float3 operator*(float3 _l, const float3& _r)
	{
		return _l.Multiply(_r);
	}

	float3 operator/(float3 _l, const float3& _r)
	{
		return _l.Divide(_r);
	}

	float3 operator+(float3 _l, float _f)
	{
		return _l.Add(_f);
	}

	float3 operator-(float3 _l, float _f)
	{
		return _l.Subtract(_f);
	}

	float3 operator*(float3 _l, float _f)
	{
		return _l.Multiply(_f);
	}

	float3 operator/(float3 _l, float _f)
	{
		return _l.Divide(_f);
	}

	float3 operator-(const float3& _v)
	{
		return float3(-_v.x, -_v.y, _v.z);
	}

	bool float3::operator==(const float3& _o) const
	{
		return (x == _o.x && y == _o.y && z == _o.z);
	}

	bool float3::operator!=(const float3& _o) const
	{
		return !(*this == _o);
	}

	float3& float3::operator+=(const float3& _o)
	{
		return Add(_o);
	}

	float3& float3::operator-=(const float3& _o)
	{
		return Subtract(_o);
	}

	float3& float3::operator*=(const float3& _o)
	{
		return Multiply(_o);
	}

	float3& float3::operator/=(const float3& _o)
	{
		return Divide(_o);
	}

	float3& float3::operator+=(float _o)
	{
		return Add(_o);
	}

	float3& float3::operator-=(float _o)
	{
		return Subtract(_o);
	}

	float3& float3::operator*=(float _o)
	{
		return Multiply(_o);
	}

	float3& float3::operator/=(float _o)
	{
		return Divide(_o);
	}

	bool float3::operator<(const float3 & _o) const
	{
		return (x < _o.x && y < _o.y && z < _o.z);
	}
	bool float3::operator<=(const float3 & _o) const
	{
		return (x <= _o.x && y <= _o.y && z <= _o.z);
	}
	bool float3::operator>(const float3 & _o) const
	{
		return (x > _o.x && y > _o.y && z > _o.z);
	}
	bool float3::operator>=(const float3 & _o) const
	{
		return (x >= _o.x && y >= _o.y && z >= _o.z);
	}

	float3 float3::Normalize() const
	{
		float length = Magnitude();
		return float3(x / length, y / length, z / length);
	}

	float3 float3::Cross(const float3 & _o) const
	{
		return float3(y * _o.z - z * _o.y, z * _o.x - x * _o.z, x * _o.y - y * _o.x);
	}

	float float3::Dot(const float3& _o) const
	{
		return (x * _o.x + y * _o.y + z * _o.z);
	}

	float float3::Magnitude() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	float float3::Distance(const float3& _o) const
	{
		float a = x - _o.x;
		float b = y - _o.y;
		float c = z - _o.z;
		return sqrt(a * a + b * b + c + c);
	}

	std::string float3::ToString() const
	{
		std::stringstream result;
		result << "float3(" << x << ", " << y << ", " << z << ")";
		return result.str();
	}
}