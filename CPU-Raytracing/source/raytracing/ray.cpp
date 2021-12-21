#include "./raytracing/ray.h"

namespace CRT
{
	Ray Ray::Reflect(float3 _n)
	{
		float3 r = D - 2 * (D.Dot(_n)) * _n;
		return Ray(O, r);
	}

	Ray Ray::Transform(const glm::mat4x4& _matrix) const
	{
		glm::vec4 origin(O.x, O.y, O.z, 1.0f);
		glm::vec4 dir(D.x, D.y, D.z, 0.0f);
		glm::vec4 whatever(1.0f, 2.0f, 3.0f, 4.0f);

		glm::vec3 n0 = origin*_matrix;
		glm::vec3 d0 = dir*_matrix;
		glm::vec4 w0 = whatever * _matrix;

		d0 = glm::normalize(d0);

		return Ray(float3(n0.x, n0.y, n0.z), float3(d0.x, d0.y, d0.z));
	}

	float3 Ray::Sample(float _t) const
	{
		return O + D * _t;
	}
}
