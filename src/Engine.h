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

			/* actual world bounds (centered at 0) */
			float aspectRatio = float(window_width) / float(window_height);
			float worldHalfHeight = 100.0f; // From projection: 1/0.01 = 100, it's hardcoded in graphics(?)
			float worldHalfWidth = worldHalfHeight * aspectRatio;  // ~133 for 800x600
		};


		Engine(Config config);
		~Engine();

		void Startup(Config config);
		void Stop();
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
		SceneManager* scene;
		bool running;

	private:
		Config config;
	};
}
