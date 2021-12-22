#include "raytracer.h"

#include <./core/graphics/screen/surface.h>
#include <./raytracing/camera.h>
#include <./raytracing/scene.h>
#include <random>
#include <functional>

namespace CRT
{
	Raytracer::Raytracer(Surface& _surface, const Scene& _scene, const Camera& _camera) :
		m_Surface(_surface),
		m_Scene(_scene),
		m_Camera(_camera),
		m_JobManager([]() { return RandomGenerator(std::random_device()()); })
	{
		m_LastResults.reserve((uint64_t)m_Surface.GetWidth() * m_Surface.GetHeight());
	}

	void Raytracer::RenderFrame()
	{
		for (uint32_t y = 0; y < m_Surface.GetHeight(); y += JobWidth)
		{
			for (uint32_t x = 0; x < m_Surface.GetWidth(); x += JobWidth)
			{
				m_LastResults.emplace_back(CreateJob(x, y));
			}
		}

		for (auto& result : m_LastResults)
		{
			JobOutput output = result.get();
			for (uint32_t y = 0; y < JobWidth; y++)
			{
				for (uint32_t x = 0; x < JobWidth; x++)
				{
					// To-do: jobs can exceed our width/height, should check inside the job
					if (x + output.XMin < m_Surface.GetWidth() && y + output.YMin < m_Surface.GetHeight())
					{
						float3 color = output.Color[x + y * JobWidth];
						m_Surface.Set(x + output.XMin, y + output.YMin,
							(0xff000000 | (int(color.x * 255) << 16) | (int(color.y * 255) << 8) | int(color.z * 255)));
					}
				}
			}
		}
		m_LastResults.clear();
	}

	std::future<Raytracer::JobOutput> Raytracer::CreateJob(uint32_t _xMin, uint32_t _yMin)
	{
		std::function<JobOutput(RandomGenerator&)> func
			= 
		[this, _xMin, _yMin]
		(RandomGenerator& generator) {
			JobOutput output{ _xMin, _yMin };
			for (uint32_t job_x = 0; job_x < JobWidth; job_x++)
			{
				for (uint32_t job_y = 0; job_y < JobWidth; job_y++)
				{
					float3 color(0.0f);
					for (uint32_t k = 0; k < m_Camera.GetAntiAliasing(); k++)
					{
						float ur = generator.NextFloat() - 0.5f;
						float vr = generator.NextFloat() - 0.5f;

						if (m_Camera.GetAntiAliasing() == 1)
						{
							vr = 0.0f;
							ur = 0.0f;
						}

						float u = (((float)job_x + _xMin) + ur) / (m_Surface.GetWidth() - 1.0f);
						float v = (((float)job_y + _yMin) + vr) / (m_Surface.GetHeight() - 1.0f);

						color += m_Scene.Intersect(m_Camera.ConstructRay({ u, v }));
					}
					output.Color[job_x + job_y * JobWidth] = color / (float)m_Camera.GetAntiAliasing();
				}
			}
			return output;
		};
		return m_JobManager.AddJob(std::move(func));
	}
}