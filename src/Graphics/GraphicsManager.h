#pragma once
#include "../Engine.h"

namespace willengine
{
	class Engine;
	struct Config;


	class GraphicsManager
	{
		friend class InputManager;

	public:
		GraphicsManager(Engine* engine);
		~GraphicsManager();

		void Startup(Engine::Config config);
		void Shutdown();
		void Draw();
		bool ShouldQuit();

	private:
		Engine* engine;
		GLFWwindow* window;

	};
}
