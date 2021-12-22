#include "raytracer.h"

#include <./core/graphics/screen/surface.h>
#include <./raytracing/camera.h>
#include <./raytracing/scene.h>
#include <./core/random_generator.h>
#include <random>
#include <functional>

namespace CRT
{
	Raytracer::Raytracer(Surface& _surface, const Scene& _scene, const Camera& _camera) :
		m_Surface(_surface),
		m_Scene(_scene),
		m_Camera(_camera)
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
			for (uint32_t jobID = 0; jobID < JobWidth * JobWidth; jobID += JOB_INC)
			{
				float3 color(0.0f);
				color += m_Scene.Intersect(m_Camera.ConstructRay(jobID, _xMin, _yMin));

				uint32_t x, y;
				morton_to_xy(jobID, &x, &y);
				output.Color[x + y * JobWidth] = color;
				
			}
			return output;
		};
		return m_JobManager.AddJob(std::move(func));
	}
}
