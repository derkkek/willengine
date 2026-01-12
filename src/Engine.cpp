#include "Engine.h"
#include "Graphics/GraphicsManager.h"
#include "Input/InputManager.h"
#include "ResourceManager/ResourceManager.h"
#include "ScriptManager/ScriptManager.h"
#include <iostream>

namespace willengine
{
	Engine::Engine()
		: graphics(new GraphicsManager(this)), 
		  input(new InputManager(this)),
		  resource(new ResourceManager(this)),
		  script(new ScriptManager(this)),
		  running(false)
	{
	}

	Engine::~Engine()
	{
		delete graphics;
		delete input;
		delete resource;
	}

	void Engine::Startup(Config config)
	{
		this->config = config;
		graphics->Startup(config);
		script->Startup();
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

			//graphics->Draw();  // Clears screen; sprites should be drawn in callback

		}
	}

	void Engine::Shutdown()
	{
		running = false;
	}
}

