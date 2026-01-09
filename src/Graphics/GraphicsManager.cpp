#include "GraphicsManager.h"
#include "../Engine.h"
#include <iostream>
#include <spdlog/spdlog.h>

namespace willengine
{
	GraphicsManager::GraphicsManager(Engine* engine) : engine(engine)
	{
	}

	GraphicsManager::~GraphicsManager()
	{
	}

	void GraphicsManager::Startup(Engine::Config config)
	{
		glfwInit();
		// We don't want GLFW to set up a graphics API.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		// Create the window.
		window = glfwCreateWindow(config.window_width, config.window_height, config.window_name.c_str(), config.window_fullscreen ? glfwGetPrimaryMonitor() : 0, 0);
		if (!window)
		{
			spdlog::error("window initialization has been failed.");
			glfwTerminate();
		}
		glfwSetWindowAspectRatio(window, config.window_width, config.window_height);
	}

	void GraphicsManager::Shutdown()
	{
		glfwTerminate();
	}
	void GraphicsManager::Draw()
	{

	}
	bool GraphicsManager::ShouldQuit()
	{
		return glfwWindowShouldClose(window);
	}
}
