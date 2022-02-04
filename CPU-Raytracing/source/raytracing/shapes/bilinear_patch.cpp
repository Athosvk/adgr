#include "bilinear_patch.h"

#include <./raytracing/manifest.h>

namespace CRT
{
	BilinearPatch::BilinearPatch(float3 _lowerLeft, float3 _lowerRight, float3 _upperLeft, float3 _upperRight) :
		LowerLeft(_lowerLeft),
		LowerRight(_lowerRight),
		UpperLeft(_upperLeft),
		UpperRight(_upperRight),
		Normal((_lowerRight - _lowerLeft).Cross(_upperRight - _upperLeft))
	{
	}

	bool BilinearPatch::Intersect(Ray _ray, Manifest& _manifest) const
	{
		float3 eLower = LowerRight - LowerLeft;
		float3 eMidRight = UpperRight - LowerRight;
		float3 eMidLeft = UpperLeft - LowerLeft;

		float3 q00 = LowerLeft - _ray.O;
		float3 q10 = LowerRight - _ray.O;

		// Equation is a + b * u + c * u^2
		float a = q00.Cross(_ray.D).Dot(eMidLeft);
		float b = q10.Cross(_ray.D).Dot(eMidRight);
		float c = Normal.Dot(_ray.D);

		b -= a + c;
		float determinant = b * b - 4 * a * c;

		if (determinant < 0.f)
		{
			return false;
		}
		determinant = std::sqrtf(determinant);
		Manifest manifest;

		// Two numerical solutions
		float u1, u2;
		if (c == 0)
		{
			u1 = -a / b;
			u2 = -1;
		}
		else
		{
			u1 = (-b - copysignf(determinant, b)) / 2;
			u2 = a / u1;
			u1 /= c;
		}

		// Test to see if it lies within the patch that matches
		// the first potential u-coordinate
		if (u1 >= 0.f && u1 <= 1.f)
		{
			float3 pa = float3::Lerp(q00, q10, u1);
			float3 pb = float3::Lerp(eMidLeft, eMidRight, u1);
			float3 n = _ray.D.Cross(pb);
			determinant = n.MagnitudeSquared();
			n = n.Cross(pa);

			float t1 = n.Dot(pb) / determinant;
			float v1 = n.Dot(_ray.D);
			if (t1 > 0.f && t1 < manifest.T && v1 >= 0.f && v1 <= determinant)
			{
				// Normals are not initialized
				manifest.T = t1;
				manifest.IntersectionPoint = _ray.Sample(manifest.T);
				manifest.UV = { u1, v1 / determinant };
			}
		}
		
		if (u2 >= 0.f && u2 <= 1.f)
		{
			float3 pa = float3::Lerp(q00, q10, u2);
			float3 pb = float3::Lerp(eMidLeft, eMidRight, u2);
			float3 n = _ray.D.Cross(pb);
			determinant = n.MagnitudeSquared();
			n = n.Cross(pa);

			float t2 = n.Dot(pb) / determinant;
			float v2 = n.Dot(_ray.D);
			if (t2 > 0.f && t2 < manifest.T && v2 >= 0.f && v2 <= determinant)
			{
				// Normals are not initialized
				manifest.T = t2;
				manifest.IntersectionPoint = _ray.Sample(manifest.T);
				manifest.UV = { u2, v2 / determinant };
			}
		}

		if (manifest.T < _manifest.T)
		{
			_manifest = manifest;
			return true;
		}

		return false;
	}
}
