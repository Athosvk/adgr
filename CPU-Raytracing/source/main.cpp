#include <iostream>

#include "./core/window/window.h"
#include "./core/graphics/render_device.h"

#include "./raytracing/scene.h"

#include "./imgui/imgui.h"
#include "./imgui/imgui_impl_glfw.h"
#include "./imgui/imgui_impl_opengl3.h"

using namespace CRT;

int main(char** argc, char** argv)
{
	// Create and Show Window
	Window* window = new Window("Title", 1280, 720);

	// Render Stuff
	RenderDevice* renderDevice = new RenderDevice(window);

	Surface* surface = new Surface(window->GetWidth(), window->GetHeight());

	Scene* scene = new Scene();
	scene->AddShape(new Plane(float3(0.0f, -2.0f, 0.0f), float3(0.0f, 1.0f, 0.0f)), new Material(float3(0.2f, 0.8f, 0.2f), 0.0f));
	scene->AddShape(new Sphere(float3(0.0f, 0.0f, -10.0f), 1.0f), new Material(float3(0.2f, 0.2f, 0.8f), 1.0f));

	// Camera
	float aspect = float(window->GetWidth()) / float(window->GetHeight());
	
	float3 E = float3(0.0f, 0.0f,  0.0f);
	float3 V = float3(0.0f, 0.0f, -1.0f);
	
	float d = 1.0f; // Scales FOV
	float3 C = E + d * V;
	
	float3 p0 = C + float3(-1,  1, 0);
	float3 p1 = C + float3( 1,  1, 0);
	float3 p2 = C + float3(-1, -1, 0);

	// IMGUI
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplOpenGL3_Init("#version 100");
	ImGui_ImplGlfw_InitForOpenGL(window->GetWindow(), true);
	ImGui::StyleColorsDark();

	bool showImgui = true;

	// Main Loop
	while (!window->ShouldClose())
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (showImgui)
		{
			ImGui::Begin("Another Window", &showImgui);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::SliderFloat3("CameraPosition", &E.x, -10.0f, 10.0f);
			ImGui::End();

			C = E + d * V;

			p0 = C + float3(-1, 1, 0);
			p1 = C + float3(1, 1, 0);
			p2 = C + float3(-1, -1, 0);

			std::cout << "p0x: " << p0.ToString() << std::endl;
			std::cout << "p1x: " << p1.ToString() << std::endl;
			std::cout << "p2x: " << p2.ToString() << std::endl;
		}

		for (uint32_t y = 0; y < 720; y++)
		{
			for (uint32_t x = 0; x < 1280; x++)
			{  
				float u = (x / (1280.0f - 1.0f));
				float v = (y / (720.0f - 1.0f));
				float3 uv = p0 + (u * (p1 - p0)) + (v * (p2 - p0));
				uv.x *= aspect;
				float3 d = (uv - E).Normalize();
				
				Manifest m = scene->Intersect(Ray(E, d));
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
	return 0;
}