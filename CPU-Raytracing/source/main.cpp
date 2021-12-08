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
#include "raytracing/raytracer.h"

using namespace CRT;

int main(char** argc, char** argv)
{
	// Create and Show Window
	Window* window = new Window("Title", 1020, 720);

	// Render Stuff
	RenderDevice* renderDevice = new RenderDevice(window);

	Texture* texture = new Texture("./assets/test_texture.png");

	Scene* scene = new Scene();
	Material* material = new Material(Color::White, 0.0f, texture);
	Material* spec_material = new Material(Color::White, 1.f, nullptr);
	Material* dielectric = new Material(Color::White, 0.0f, nullptr);
	dielectric->type = Type::Dielectric;
	dielectric->RefractionIndex = 1.52f;
	
	//scene->AddShape(new Plane(float3(0.0f, -5.0f, 0.0f), float3(0.0f, -1.0f, 0.0f)), new Material(Color::White, 0.0f, texture));
	scene->AddShape(new Plane(float3(7.0f, 0.0f, 0.0f), float3(-1.0f, 0.0f, 0.0f)), new Material(Color::Blue, 0.0f, nullptr));
	scene->AddShape(new Plane(float3(-7.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 0.0f)), new Material(Color::Red, 0.0f, nullptr));
	scene->AddShape(new Plane(float3(0.0f, -5.0f, 0.0f), float3(0.0f, 1.0f, 0.0f)), material);
	scene->AddShape(new Plane(float3(0.0f, 5.0f, 0.0f), float3(0.0f, -1.0f, 0.0f)), new Material(float3{ 0.3f,0.3f,0.3f }, 0.0f, nullptr));
	scene->AddShape(new Plane(float3(0.0f, 0.0f, -12.f), float3(0.0f, 0.0f, 1.0f)), new Material(Color::White, 0.0f, nullptr));


	ModelLoading::LoadModel(scene, material, float3(0.0f, 0.0f, -2.0f), "./assets/box.obj");

	//scene->AddShape(new Sphere(float3(-1.0f, -3.f, -4.0f), 2.f), spec_material);
	//scene->AddShape(new Sphere(float3(3.0f, -3.f, -2.0f), 2.f), material);
	scene->AddDirectionalLight(DirectionalLight{ float3(0.0f, 0.f, -1.f).Normalize(), 0.5f, Color::White });
	scene->AddPointLight(PointLight{ float3(0.0f, 4.5f, 0.f), 3500.0f, Color::White });
	scene->AddPointLight(PointLight{ float3(-2.0f, 4.0f, -1.5f), 15000.0f, Color::Blue });
	//scene->AddSpotLight(SpotLight{ float3(0.0f, 4.99f, -3.0f), float3(0.0f, -1.0f, 0.0f).Normalize(), 0.99f, 0.82f, 125.0f, Color::Purple });
	//scene->AddSpotLight(SpotLight{ float3(0.0f, 4.99f, -3.0f), float3(0.0f, 0.0f, -1.0f).Normalize(), 0.99f, 0.82f, 125.0f, Color::Purple });

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
	auto position = camera.GetPosition();
		camera.SetPosition({ position.x, position.y, 18.0f });
	// Main Loop
	float previousFrame = (float)glfwGetTime();

	Surface surface(window->GetWidth(), window->GetHeight());
	Raytracer raytracer(surface, *scene, camera);
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
		
		raytracer.RenderFrame();
		renderDevice->CopyFrom(&surface);
		renderDevice->Present();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window->PollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	delete renderDevice;
	delete window;
	delete scene;
	return 0;
}
