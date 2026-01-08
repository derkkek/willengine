#pragma once

#include "Types.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <string>



namespace willengine
{
	class Engine
	{
	public:

		struct Config
		{
			// Window settings
			int window_width = 800;
			int window_height = 600;
			std::string window_name = "WillEngine";
			bool window_fullscreen = false;
		};


		Engine();
		~Engine();

		void Startup(Config config);
		void Shutdown();
		void RunGameLoop(void(*update)());

		GraphicsManager* graphics;
		InputManager* input;

	private:
		bool running;
		Config config;
	};
}
