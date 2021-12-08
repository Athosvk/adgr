#include <iostream>
#include <array>

#include "./core/window/window.h"
#include "./core/graphics/render_device.h"
#include "./core/graphics/color3.h"

#include "./raytracing/scene.h"
#include "./raytracing/camera.h"

#include "./imgui/imgui.h"
#include "./imgui/imgui_impl_glfw.h"
#include "./imgui/imgui_impl_opengl3.h"

#include "./scene/camera_controller.h"

#include "./scene/model_loading.h"
#include "job_manager.h"

using namespace CRT;

int main(char** argc, char** argv)
{
	// Create and Show Window
	Window* window = new Window("Title", 720, 480);

	// Render Stuff
	RenderDevice* renderDevice = new RenderDevice(window);

	Surface* surface = new Surface(window->GetWidth(), window->GetHeight());

	Texture* texture = new Texture("./assets/test_texture.png");

	Scene* scene = new Scene();
	// Leaks
	Material* material = new Material(float3(0.2f, 0.2f, 0.8f), 0.0f, nullptr);
	Material* spec_material = new Material(Color::White, 0.9f, nullptr);
	Material* dielectric = new Material(Color::Red, 0.9f, nullptr);
	dielectric->type = Type::Dielectric;
	dielectric->RefractionIndex = 1.4f;
	scene->AddShape(new Plane(float3(0.0f, -5.0f, 0.0f), float3(0.0f, 1.0f, 0.0f)), new Material(float3(0.2f, 0.8f, 0.2f), 0.0f, texture));
	// scene->AddShape(new Plane(float3(0.0f, -3.0f, 0.0f), float3(0.0f, 1.0f, 0.0f)), dielectric);
	// Leaks

	ModelLoading::LoadModel(scene, float3(0.0f, -2.0f, -8.0f), "./assets/box.obj");

	scene->AddShape(new Sphere(float3(2.0f, -1.0f, -7.0f), 1.f), material);
	scene->AddShape(new Sphere(float3(5.0f, -1.0f, -7.0f), 1.f), dielectric);
	scene->AddDirectionalLight(DirectionalLight{ float3(0.0f, -1.f, 0.f).Normalize(), 0.5f, Color::White });
	//scene->AddDirectionalLight(DirectionalLight{ float3(0.0f, -.75f, 0.75f).Normalize(), 0.2f, Color::White });
	//scene->AddPointLight(PointLight{ float3(3.5f, 0.5f, -8.5f), 125.0f, Color::Yellow });
	//scene->AddPointLight(PointLight{ float3(3.5f, 0.5f, -11.5f), 125.0f, Color::Green });
	//scene->AddPointLight(PointLight{ float3(0.5f, 1.5f, -10.5f), 125.0f, Color::Purple });
	//scene->AddPointLight(PointLight{ float3(1.f, 0.5f, -11.5f), 125.0f, Color::Red });

	scene->AddSpotLight(SpotLight{ float3(0.0f, 10.0f, -10.0f), float3(0.0f, -1.0f, 0.0f).Normalize(), 0.91f, 0.82f, 125.0f, Color::Purple });

	// Camera
	float aspect = float(window->GetWidth()) / float(window->GetHeight());
	
	// IMGUI
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_Init("#version 100");
	ImGui_ImplGlfw_InitForOpenGL(window->GetWindow(), true);
	ImGui::StyleColorsDark();

	bool showImgui = true;

	float2 viewport(window->GetWidth(), window->GetHeight());
	Camera camera(viewport);
	CameraController controller(camera);
	// Main Loop
	float previousFrame = (float)glfwGetTime();

	constexpr unsigned JobWidth = 4;
	struct JobOutput
	{
		uint32_t XMin;
		uint32_t YMin;
		std::array<float3, JobWidth * JobWidth> Color;
	};

	JobManager job_manager;
	std::vector<std::future<JobOutput>> results;
	results.reserve((int32_t)(viewport.x * viewport.y));
	while (!window->ShouldClose())
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float currentFrame = (float)glfwGetTime();
		float deltaTime = currentFrame - previousFrame;
		previousFrame = currentFrame;

		if (showImgui)
		{
			ImGui::Begin("Window", &showImgui);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

			ImGui::Text("FPS: %.2f", 1.0 / deltaTime);
			ImGui::Separator();
			if (ImGui::CollapsingHeader("Camera"))
			{
				float3 cameraPosition = camera.GetPosition();
				float3 cameraRight = camera.GetRight();
				float3 cameraUp = camera.GetUp();
				float3 cameraFront = camera.GetFront();
				float focalLength = camera.GetFocalLength();
				ImGui::SliderFloat3("CameraPosition", &cameraPosition.x, -10.0f, 10.0f);
				ImGui::SliderFloat3("CameraRight", &cameraRight.x, -10.0f, 10.0f);
				ImGui::SliderFloat3("CameraUp", &cameraUp.x, -10.0f, 10.0f);
				ImGui::SliderFloat3("CameraFront", &cameraFront.x, -10.0f, 10.0f);
				ImGui::SliderFloat("FocalLength", &focalLength, 0.5f, 5.0f);
				camera.SetPosition(cameraPosition);
				camera.SetFocalLength(focalLength);
			}
			ImGui::End();
		}
		controller.ProcessInput(window->GetWindow(), deltaTime);
		
		uint32_t aa = camera.GetAntiAliasing();
		constexpr unsigned JobWidth = 4;

		for (uint32_t y = 0; y < viewport.y; y += JobWidth)
		{
			for (uint32_t x = 0; x < viewport.x; x += JobWidth)
			{
				results.push_back(job_manager.AddJob([=] {
					JobOutput output{ x, y };
					for (uint32_t job_x = 0; job_x < JobWidth; job_x++)
					{
						for (uint32_t job_y = 0; job_y < JobWidth; job_y++)
						{
							float3 color(0.0f);
							for (uint32_t k = 0; k < aa; k++)
							{
								float ur = (rand() / (RAND_MAX + 1.0f)) - 0.5f;
								float vr = (rand() / (RAND_MAX + 1.0f)) - 0.5f;

								if (aa == 1)
								{
									vr = 0.0f;
									ur = 0.0f;
								}

								float u = ((((float)job_x + x) + ur) / (viewport.x - 1.0f));
								float v = ((((float)job_y + y) + vr) / (viewport.y - 1.0f));

								color += scene->Intersect(camera.ConstructRay({ u, v }));
							}
							output.Color[job_x + job_y * JobWidth] = color;
						}
					}
					return output;
				}));
			}
		}

		for (auto& result : results)
		{
			JobOutput output = result.get();
			for (uint32_t y = 0; y < JobWidth; y++)
			{
				for (uint32_t x = 0; x < JobWidth; x++)
				{
					if (x < viewport.x && y < viewport.y)
					{
						float3 color = output.Color[x + y * JobWidth];
						surface->Set(x + output.XMin, y + output.YMin,
							(0xff000000 | (int(color.x * 255) << 16) | (int(color.y * 255) << 8) | int(color.z * 255)));
					}
				}
			}
		}
		
		renderDevice->CopyFrom(surface);
		renderDevice->Present();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window->PollEvents();
		results.clear();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	delete renderDevice;
	delete window;
	delete surface;
	delete scene;
	return 0;
}
