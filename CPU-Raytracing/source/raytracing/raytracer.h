#pragma once
#include <./core/job_manager.h>
#include <cstdint>

#include <array>

namespace CRT
{
	class Surface;
	class Camera;
	class Scene;

	class Raytracer
	{
	private:
		constexpr static uint32_t JobWidth = 16;

		struct JobOutput
		{
			uint32_t XMin;
			uint32_t YMin;
			std::array<float3, JobWidth * JobWidth> Color;
		};

	public:
		Raytracer(Surface& _surface, const Scene& scene, const Camera& _camera);
		void RenderFrame();
	private:
		std::future<JobOutput> CreateJob(uint32_t _xMin, uint32_t _yMin);

		Surface& m_Surface;
		const Scene& m_Scene;
		const Camera& m_Camera;
		JobManager m_JobManager; //= JobManager(1);

		// Saves a big allocation every frame
		std::vector<std::future<JobOutput>> m_LastResults;
	};
}
