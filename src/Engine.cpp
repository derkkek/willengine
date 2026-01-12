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
		delete script;
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

			graphics->Draw(graphics->sprites); //passing it's sprites to it's function weird, i could refactor in the future.

		}
	}

	void Engine::Shutdown()
	{
		running = false;
	}
}

