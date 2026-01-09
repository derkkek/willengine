#include "Engine.h"
#include "Graphics/GraphicsManager.h"
#include "Input/InputManager.h"
#include <iostream>

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

	void Engine::RunGameLoop(const UpdateCallback& callback)
	{

					
		double now = glfwGetTime();
		double timePerExecution = 1.0 / 60.0;
		double lastTick = now - timePerExecution;

		while (running && !graphics->ShouldQuit())
		{
			now = glfwGetTime();
			while (now >= lastTick + timePerExecution)
			{
				input->Update();
				callback();
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

