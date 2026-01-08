#include "Engine.h"
#include "Graphics/GraphicsManager.h"
#include "Input/InputManager.h"

namespace willengine
{
	Engine::Engine()
		: graphics(new GraphicsManager(this)), 
		  input(new InputManager(this)),
		  running(false)
	{
	}

	Engine::~Engine()
	{
		delete graphics;
		delete input;
	}

	void Engine::Startup(Config config)
	{
		this->config = config;
		graphics->Startup(config);
		running = true;
	}

	void Engine::RunGameLoop(void(*UpdateCallback)())
	{

					
		double now = glfwGetTime();
		double timePerExecution = 1000 / 60000;
		double lastTick = now - timePerExecution;

		while (running)
		{
			now = glfwGetTime();
			while (now >= lastTick + timePerExecution)
			{
				//input update;
				UpdateCallback();
				lastTick += timePerExecution;
			}

			graphics->Draw();

		}
	}

	void Engine::Shutdown()
	{
		running = false;
	}
}

