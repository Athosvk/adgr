#include <iostream>

#include "./core/window/window.h"
#include "./core/graphics/render_device.h"

#include "./raytracing/scene.h"
#include "./raytracing/camera.h"

#include "./imgui/imgui.h"
#include "./imgui/imgui_impl_glfw.h"
#include "./imgui/imgui_impl_opengl3.h"

#include "./scene/camera_controller.h"

using namespace CRT;

int main(char** argc, char** argv)
{
	// Create and Show Window
	Window* window = new Window("Title", 1280, 720);

	// Render Stuff
	RenderDevice* renderDevice = new RenderDevice(window);

	Surface* surface = new Surface(window->GetWidth(), window->GetHeight());

	Scene* scene = new Scene();
	// Leaks
	scene->AddShape(new Plane(float3(0.0f, -2.0f, 0.0f), float3(0.0f, 1.0f, 0.0f)), new Material(float3(0.2f, 0.8f, 0.2f), 0.0f));
	// Leaks
	scene->AddShape(new Sphere(float3(1.0f, 0.0f, -10.0f), 1.0f), new Material(float3(0.2f, 0.2f, 0.8f), 1.0f));

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

		for (uint32_t y = 0; y < viewport.y; y++)
		{
			for (uint32_t x = 0; x < viewport.x; x++)
			{  
				float u = (x / (viewport.x - 1.0f));
				float v = (y / (viewport.y - 1.0f));
				
				Manifest m = scene->Intersect(camera.ConstructRay({ u, v }));
				if (m.M != nullptr)
				{
					uint32_t color = (0xff000000 | (int(m.M->Color.x * 255) << 16) | (int(m.M->Color.y * 255) << 8) | int(m.M->Color.z * 255));
					surface->Set(x, y, color);
				}
				else
					surface->Set(x, y, 0xff000000);
			}
		}
		
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