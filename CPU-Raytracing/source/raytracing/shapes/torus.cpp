#include "./raytracing/shapes/torus.h"
#include "./core/math/trigonometry.h"
#include "./core/math/poly34.h"

#include <vector>

#include <algorithm>
#include <cmath>
#include <complex>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_precision.hpp>

#define M_PI 3.14159265358979323846

namespace CRT
{
	Torus::Torus(float3 _pos, float _r1, float _r2)
		: Shape(ShapeType::SHAPE_TYPE_TORUS)
		, RR1(_r1)
		, RR2(_r2)
		, Position(_pos)
	{
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(Position.x, Position.y, Position.z));

		invModel = glm::inverse(model);
		invModel = glm::transpose(invModel);
	}

	bool Torus::Intersect(Ray _r, Manifest& _m)
	{
		const auto transformedRay = _r.Transform(invModel);
		const auto transformedRayDir = transformedRay.D;
		const auto transformedOrigin = transformedRay.O;

		float rayDirDot = transformedRayDir.Dot(transformedRayDir);
		float originDot = transformedOrigin.Dot(transformedOrigin);

		float originDirDot = transformedRayDir.Dot(transformedOrigin);
		float commonTerm = originDot - (RR1 * RR1) - (RR2 * RR2);

		float c4 = rayDirDot * rayDirDot;
		float c3 = 4.0f * rayDirDot * originDirDot;
		float c2 = 2.0f * rayDirDot * commonTerm + 4.0f * originDirDot * originDirDot + 4.0f * (RR1 * RR1) * transformedRayDir.y * transformedRayDir.y;
		float c1 = 4.0f * commonTerm * originDirDot + 8.0f * (RR1 * RR1) * transformedOrigin.y * transformedRayDir.y;
		float c0 = commonTerm * commonTerm - 4.0f * (RR1 * RR1) * ((RR2 * RR2) - transformedOrigin.y * transformedOrigin.y);

		c3 /= c4;
		c2 /= c4;
		c1 /= c4;
		c0 /= c4;

		std::complex<double>* solutions = solve_quartic(c3, c2, c1, c0);

		float minRealRoot = FLT_MAX;
		if (solutions[0].real() > 0.0f && solutions[0].imag() == 0.0f && solutions[0].real() < minRealRoot) {
			minRealRoot = solutions[0].real();
		}

		if (solutions[1].real() > 0.0f && solutions[1].imag() == 0.0f && solutions[1].real() < minRealRoot) {
			minRealRoot = solutions[1].real();
		}

		if (solutions[2].real() > 0.0f && solutions[2].imag() == 0.0f && solutions[2].real() < minRealRoot) {
			minRealRoot = solutions[2].real();
		}

		if (solutions[3].real() > 0.0f && solutions[3].imag() == 0.0f && solutions[3].real() < minRealRoot) {
			minRealRoot = solutions[3].real();
		}

		delete[] solutions;

		if (minRealRoot == FLT_MAX) {
			return false;
		}

		//	if (minRealRoot < _m.T) {

		auto localp = transformedRay.Sample(minRealRoot);
		float sumSquared = localp.Dot(localp);
		float radii = RR1 * RR1 + RR2 * RR2;
		glm::fvec3 normal = glm::normalize(glm::fvec3(
			4.0f * localp.x * (sumSquared - radii),
			4.0f * localp.y * (sumSquared - radii + 2.0f * (RR1 * RR1)),
			4.0f * localp.z * (sumSquared - radii)
		));

		auto transformedNormal = model * glm::fvec4(normal, 0);
		//transformedNormal = -transformedNormal;

		_m.IntersectionPoint = localp;
		_m.T = minRealRoot;
		_m.UV = GetUV(localp, float3(1.0f, 1.0f, 1.0).Normalize());
		_m.N = float3(transformedNormal.x, transformedNormal.y, transformedNormal.z).Normalize();
		//getUV(rec);
		return true;
		//	}

		//	return false;
	}
	float2 Torus::GetUV(float3 _point, float3 _normal)
	{
		float u = 0.5f + (std::atan2(_point.z, _point.x) / M_2PI);
		float v = 0.5f + (std::atan2(_point.y, (sqrt(_point.x * _point.x + _point.z * _point.z) - RR1)) / M_2PI);
		return float2(u, v);
	}
}
