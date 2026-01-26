#pragma once
#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>
namespace willeditor
{
	class EventHandler;
	class UI;
	class App
	{
	public:
		App();
		~App()=default;
		void Startup(GLFWwindow* window, WGPUDevice device, WGPUTextureFormat render_target_format);
		void Update();
		void Shutdown();
		UI* ui;
		EventHandler* eventHandler;
	private:

	};

}

