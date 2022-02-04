#include "./core/math/float2.h"
#include "./core/math/float3.h"
#include "./core/math/float4.h"
#include <smmintrin.h>
#include <immintrin.h>

#include <sstream>
#include <algorithm>

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

	float3 float3::Infinity()
	{
		return float3(std::numeric_limits<float>::infinity());
	}

	float3 float3::NegativeInfinity()
	{
		return -Infinity();
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

	float3 float3::ComponentMin(std::initializer_list<float3> values)
	{
		auto it = values.begin();
		__m128 minM128 = _mm_setr_ps(it->x, it->y, it->z, 0.0f);
		for (; it != values.end(); it++)
		{
			minM128 = _mm_min_ps(minM128, _mm_setr_ps(it->x, it->y, it->z, 0.0f));
		}
		float4 min;
		_mm_storeu_ps(min.f, minM128);
		return min;
	}

	float3 float3::ComponentMax(std::initializer_list<float3> values)
	{
		auto it = values.begin();
		__m128 maxM128 = _mm_setr_ps(it->x, it->y, it->z, 0.0f);
		for (; it != values.end(); it++)
		{
			maxM128 = _mm_max_ps(maxM128, _mm_setr_ps(it->x, it->y, it->z, 0.0f));
		}
		float4 max;
		_mm_storeu_ps(max.f, maxM128);
		return max;
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
		return float3(-_v.x, -_v.y, -_v.z);
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
		return sqrt(MagnitudeSquared());
	}

	float float3::Distance(const float3& _o) const
	{
		return sqrt(DistanceSquared(_o));
	}

	float float3::MagnitudeSquared() const
	{
		return Dot(*this);
	}

	float3 float3::ComponentMin(float3 other) const
	{
		return { std::min(x, other.x), std::min(y, other.y),
			std::min(z, other.z) };
	}

	float3 float3::ComponentMax(float3 other) const
	{
		return { std::max(x, other.x), std::max(y, other.y),
			std::max(z, other.z) };
	}

	float3 float3::Abs() const
	{
		return float3(std::abs(x), std::abs(y), std::abs(z));
	}

	float3 float3::Lerp(float3 v1, float3 v2, float t)
	{
		return v1 + (v2 - v1) * t;
	}

	float float3::DistanceSquared(const float3& _o) const
	{
		return (*this - _o).MagnitudeSquared();
	}

	std::string float3::ToString() const
	{
		std::stringstream result;
		result << "float3(" << x << ", " << y << ", " << z << ")";
		return result.str();
	}
}
