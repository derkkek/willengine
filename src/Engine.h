#pragma once

#include "Types.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <string>
#include "ECS/ECS.h"
#include "EventManager/EventManager.h"


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
		void RunGameLoop(const UpdateCallback& callback);
		Config& BringEngineConfiguration();

		GraphicsManager* graphics;
		PhysicsManager* physics;
		InputManager* input;
		ResourceManager* resource;
		ScriptManager* script;
		ECS ecs;
		EventManager* event;
		SoundManager* sound;

	private:
		bool running;
		Config config;
	};
}
