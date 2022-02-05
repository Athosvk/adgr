#include <iostream>

#include "./core/window/window.h"
#include "./core/graphics/render_device.h"
#include "./core/graphics/color3.h"

#include "./raytracing/camera.h"
#include "./raytracing/raytracer.h"

#include "./imgui/imgui.h"
#include "./imgui/imgui_impl_glfw.h"
#include "./imgui/imgui_impl_opengl3.h"

#include "./scene/camera_controller.h"

#include "./scene/model_loading.h"
#include "./benchmarking/rolling_sampler.h"
#include "./benchmarking/timer.h"

using namespace CRT;

int main(char** argc, char** argv)
{
	// Create and Show Window
	Window* window = new Window("CRT", 1280, 720);

	// Render Stuff
	RenderDevice* renderDevice = new RenderDevice(window);

	Texture* texture = new Texture("./assets/texture.png");
	Texture* heightMap = new Texture("./assets/heightmap.png");

	Scene* scene = new Scene();
	Material* material = new Material(Color::White, 0.0f, nullptr);
	Material* texturedMaterial = new Material(Color::White, 0.0f, texture);
	Material* heightMapMaterial = new Material(Color::White, 0.0f, texture, heightMap);
	
	Timer::Duration bvhConstructionDuration;
	{
		Timer bvhConstructionTimer;
		ModelLoading::LoadModel(scene, heightMapMaterial, float3(0.0f, -1.0f, 0.5f), "./assets/plane.obj");
		bvhConstructionDuration = bvhConstructionTimer.GetDuration();
	}

	//scene->AddDirectionalLight(DirectionalLight{ float3(0.0f, -0.75f, -0.75f).Normalize(), 0.6f, Color::White });
	scene->AddPointLight(PointLight{ float3(0.0f, 1.0f, 0.5f), 100.0f, Color::White });
	//scene->AddPointLight(PointLight{ float3(0.0f, 1.5f, 2.5f), 31155.0f, Color::White });

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
	auto initialCameraPosition = float3 { 0.0f, 0.0f, 3.0f };
	camera.SetPosition(initialCameraPosition);

	Surface surface(window->GetWidth(), window->GetHeight());
	Raytracer raytracer(surface, *scene, camera);
	RollingSampler<Timer::Duration, 20> rtFrameSampler;
	float deltaFrameTime = 0.f;

	auto previousCameraDirection = float3(std::numeric_limits<float>::infinity());
	auto previousCameraPosition = previousCameraDirection;
	bool sceneDirty = false;
	bool staticRenderOnly = true;
	while (!window->ShouldClose())
	{
		Timer frameTimer;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		sceneDirty = !staticRenderOnly;
		if (showImgui)
		{
			ImGui::Begin("Window", &showImgui);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

			ImGui::Text("RayTracer Frametime: %.3f ms", rtFrameSampler.GetAverage().count() * 1000);
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

				ImGui::Checkbox("Static Render Only", &staticRenderOnly);
			}
			if (ImGui::CollapsingHeader("BVH"))
			{
				ImGui::Text("Last BVH construction duration: %.4f s", bvhConstructionDuration.count());
				ImGui::Text("Tris: %u", scene->GetTriangleCount());
				ImGui::Text("Nodes: %u", scene->GetBHVNodeCount());
				bool bvh = scene->IsBVHEnabled();
				if (ImGui::Checkbox("BVH Enabled", &bvh))
				{
					sceneDirty = true;
				}
				if (bvh)
				{
					scene->EnableBVH();
				}
				else
				{
					scene->DisableBVH();
				}
				
				ImGui::Separator();
				ImGui::Text("Debug draw setting");
				ETraversalDebugSetting debug_setting = scene->GetBVHDebugSetting();
				if (ImGui::RadioButton("None", debug_setting == ETraversalDebugSetting::None))
				{
					scene->SetBVHDebugSetting(ETraversalDebugSetting::None);
					sceneDirty = true;
				}
				else if (ImGui::RadioButton("Traverse Only", debug_setting == ETraversalDebugSetting::TraversalOnly))
				{
					scene->SetBVHDebugSetting(ETraversalDebugSetting::TraversalOnly);
					sceneDirty = true;
				}
				else if (ImGui::RadioButton("Blend", debug_setting == ETraversalDebugSetting::Blend))
				{
					scene->SetBVHDebugSetting(ETraversalDebugSetting::Blend);
					sceneDirty = true;
				}
				else if (ImGui::RadioButton("Exclude Hits", debug_setting == ETraversalDebugSetting::ExcludeHits))
				{
					scene->SetBVHDebugSetting(ETraversalDebugSetting::ExcludeHits);
					sceneDirty = true;
				}
			}
			ImGui::End();
		}
		if (previousCameraPosition != camera.GetPosition() || previousCameraDirection != camera.GetFront())
		{
			sceneDirty = true;
		}
		
		// Only update if our view changed
		if (sceneDirty)
		{
			Timer rtTimer;
			raytracer.RenderFrame();
			rtFrameSampler.AddSample(rtTimer.GetDuration());
			sceneDirty = false;
		}
		previousCameraPosition = camera.GetPosition();
		previousCameraDirection = camera.GetFront();

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
