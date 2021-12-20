#include <array>
#include <iostream>

#include "./core/window/window.h"
#include "./core/graphics/render_device.h"
#include "./core/graphics/color3.h"

#include "./raytracing/scene.h"
#include "./raytracing/camera.h"
#include "./raytracing/shapes/plane.h"

#include "./imgui/imgui.h"
#include "./imgui/imgui_impl_glfw.h"
#include "./imgui/imgui_impl_opengl3.h"

#include "./scene/camera_controller.h"

#include "./scene/model_loading.h"
#include "./benchmarking/rolling_sampler.h"
#include "./benchmarking/timer.h"
#include "raytracing/raytracer.h"

using namespace CRT;

int main(char** argc, char** argv)
{
	// Create and Show Window
	Window* window = new Window("CRT", 1280, 720);

	// Render Stuff
	RenderDevice* renderDevice = new RenderDevice(window);

	Texture* texture = new Texture("./assets/test_texture.png");

	Scene* scene = new Scene();
	Material* material = new Material(Color::White, 0.0f, nullptr);
	Material* spec_material = new Material(Color::White, 0.9f, nullptr);
	Material* partial_spec_material = new Material(Color::White, 0.4f, nullptr);
	Material* dielectric = new Material(Color::White, 0.0f, nullptr);
	dielectric->type = Type::Dielectric;
	dielectric->RefractionIndex = 1.52f;
	
	//scene->AddShape(new Plane(float3(7.0f, 0.0f, 0.0f), float3(-1.0f, 0.0f, 0.0f)), new Material(Color::Blue, 0.0f, nullptr));
	//scene->AddShape(new Plane(float3(-7.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 0.0f)), new Material(Color::Red, 0.0f, nullptr));
	// scene->AddShape(new Plane(float3(0.0f, -5.0f, 0.0f), float3(0.0f, 1.0f, 0.0f)), new Material(Color::White, 0.0f, nullptr));
	//scene->AddShape(new Plane(float3(0.0f, 5.0f, 0.0f), float3(0.0f, -1.0f, 0.0f)), new Material(float3{ 0.3f,0.3f,0.3f }, 0.0f, nullptr));
	//scene->AddShape(new Plane(float3(0.0f, 0.0f, -12.f), float3(0.0f, 0.0f, 1.0f)), new Material(Color::White, 0.0f, nullptr));

	ModelLoading::LoadModel(scene, material, float3(0.0f, 0.0f, 00.0f), "./assets/suzanne.obj");
	ModelLoading::LoadModel(scene, material, float3(5.0f, 0.0f, 0.0f), "./assets/suzanne.obj");
	ModelLoading::LoadModel(scene, material, float3(-5.0f, 0.0f, 0.0f), "./assets/suzanne.obj");

	Timer::Duration bvhConstructionDuration;
	{
		Timer bvhConstructionTimer;
		scene->ConstructBVH();
		bvhConstructionDuration = bvhConstructionTimer.GetDuration();
		std::cout << "Constructed BVH in " << bvhConstructionDuration.count() << " seconds";
	}

	scene->AddPointLight(PointLight{ float3(0.0f, 4.5f, 0.f), 3500.0f, Color::White });
	// scene->AddPointLight(PointLight{ float3(-2.0f, 4.0f, -1.5f), 15000.0f, Color::White });

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
	auto initialCameraPosition = float3 { 0.0f, 0.0f, 10.0f };
	camera.SetPosition(initialCameraPosition);

	Surface surface(window->GetWidth(), window->GetHeight());
	Raytracer raytracer(surface, *scene, camera);
	RollingSampler<Timer::Duration, 20> rtFrameSampler;
	float deltaFrameTime = 0.f;
	while (!window->ShouldClose())
	{
		Timer frameTimer;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (showImgui)
		{
			ImGui::Begin("Window", &showImgui);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

			ImGui::Text("RayTracer FPS: %.2f", 1.0 / rtFrameSampler.GetAverage().count());
			ImGui::Separator();
			if (ImGui::CollapsingHeader("Camera"))
			{
				float3 cameraPosition = camera.GetPosition();
				ImGui::SliderFloat3("CameraPosition", &cameraPosition.x, -10.0f, 10.0f);
				camera.SetPosition(cameraPosition);

				float fieldOfView = ToDegrees(camera.GetFieldOfView());
				ImGui::SliderFloat("Field of view", &fieldOfView, 0.1f, 179.9f);
				camera.SetFieldOfView(ToRadians(fieldOfView));

				int antiAliasing = int(camera.GetAntiAliasing());
				ImGui::SliderInt("Anti aliasing", &antiAliasing, 1, 16);
				camera.SetAntiAliasing(antiAliasing);

				if (ImGui::Button("Reset"))
				{
					camera.SetPosition(initialCameraPosition);
					controller.Reset();
				}
			}
			if (ImGui::CollapsingHeader("BVH"))
			{
				ImGui::Text("Last BVH construction duration: %.4f s", bvhConstructionDuration.count());
				ImGui::Text("Tris: %u", scene->GetTriangleCount());
				bool bvh = scene->IsBVHEnabled();
				ImGui::Checkbox("BVH", &bvh);
				if (bvh)
				{
					scene->EnableBVH();
				}
				else
				{
					scene->DisableBVH();
				}
				
				if (ImGui::Button("Reconstruct BVH"))
				{
					Timer bvhTimer;
					scene->ConstructBVH();
					bvhConstructionDuration = bvhTimer.GetDuration();
				}
			}
			ImGui::End();
		}
		
		{
			Timer rtTimer;
			raytracer.RenderFrame();
			rtFrameSampler.AddSample(rtTimer.GetDuration());
		}

		controller.ProcessInput(window->GetWindow(), deltaFrameTime);
		renderDevice->CopyFrom(&surface);
		renderDevice->Present();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window->PollEvents();
		deltaFrameTime = frameTimer.GetDuration().count();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	delete renderDevice;
	delete window;
	delete scene;
	return 0;
}
