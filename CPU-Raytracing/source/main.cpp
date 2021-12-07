#include <iostream>

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

using namespace CRT;

int indexX = 0;
int indexY = 0;
int main(char** argc, char** argv)
{
	// Create and Show Window
	Window* window = new Window("Title", 1280, 720);

	// Render Stuff
	RenderDevice* renderDevice = new RenderDevice(window);

	Surface* surface = new Surface(window->GetWidth(), window->GetHeight());

	Texture* texture = new Texture("./assets/test_texture.png");

	Scene* scene = new Scene();
	// Leaks
	scene->AddShape(new Plane(float3(0.0f, -2.0f, 0.0f), float3(0.0f, 1.0f, 0.0f)), new Material(float3(0.2f, 0.8f, 0.2f), 0.0f, texture));
	// Leaks
	Material* material = new Material(float3(0.2f, 0.2f, 0.8f), 0.5f, nullptr);
	Material* spec_material = new Material(Color::White, 0.9f, nullptr);

	//ModelLoading::LoadModel(scene, float3(0.0f, 0.0f, -15.0f), "./assets/bunny.obj");

	scene->AddShape(new Sphere(float3(2.0f, -1.0f, -7.0f), 1.f), spec_material);
	scene->AddShape(new Sphere(float3(5.0f, -1.0f, -7.0f), 1.f), spec_material);
	scene->AddDirectionalLight(DirectionalLight{ float3(0.0f, -.75f, -.75f).Normalize(), 0.2f, Color::White });
	scene->AddDirectionalLight(DirectionalLight{ float3(0.0f, -.75f, 0.75f).Normalize(), 0.2f, Color::White });
	scene->AddPointLight(PointLight{ float3(3.5f, 0.5f, -8.5f), 125.0f, Color::Yellow });
	scene->AddPointLight(PointLight{ float3(3.5f, 0.5f, -11.5f), 125.0f, Color::Green });
	scene->AddPointLight(PointLight{ float3(0.5f, 1.5f, -10.5f), 125.0f, Color::Purple });
	scene->AddPointLight(PointLight{ float3(1.f, 0.5f, -11.5f), 125.0f, Color::Red });

	// Camera
	float aspect = float(window->GetWidth()) / float(window->GetHeight());
	
	// IMGUI
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_Init("#version 100");
	ImGui_ImplGlfw_InitForOpenGL(window->GetWindow(), true);
	ImGui::StyleColorsDark();

	bool showImgui = true;

	float2 viewport(1280, 720);
	Camera camera(viewport);
	CameraController controller(camera);
	// Main Loop
	float previousFrame = (float)glfwGetTime();
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
		
		//for (uint32_t y = 0; y < viewport.y; y++)
		//{
		//	printf("Y: %i\n", y);
			for (uint32_t x = 0; x < viewport.x; x++)
			{  
			//	if (indexX >= viewport.x)
			//	{
			//		indexX = 0;
			//		indexY++;
			//		printf("Y: %i\n", indexY);
			//	}
				if (indexY >= viewport.y)
				{
					indexY = 0;
				}
			//	int x = indexX;
				int y = indexY;

				float u = (x / (viewport.x - 1.0f));
				float v = (y / (viewport.y - 1.0f));
				
				float3 color = scene->Intersect(camera.ConstructRay({ u, v }));
				surface->Set(x, y, (0xff000000 | (int(color.x * 255) << 16) | (int(color.y * 255) << 8) | int(color.z * 255)));
		//		indexX++;
			}
				indexY++;
				printf("Y: %i\n", indexY);
		//}
		
		renderDevice->CopyFrom(surface);
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
	delete surface;
	delete scene;
	return 0;
}