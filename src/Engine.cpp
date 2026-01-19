#include "Engine.h"
#include "GraphicsManager/GraphicsManager.h"
#include "InputManager/InputManager.h"
#include "ResourceManager/ResourceManager.h"
#include "ScriptManager/ScriptManager.h"
#include "SoundManager/SoundManager.h"
#include <iostream>

namespace willengine
{
	Engine::Engine()
		: graphics(new GraphicsManager(this)), 
		  input(new InputManager(this)),
		  resource(new ResourceManager(this)),
		  script(new ScriptManager(this)),
		  event(new EventManager),
	      sound(new SoundManager(this)),
		  running(false)
	{
	}

	Engine::~Engine()
	{
		delete graphics;
		delete input;
		delete resource;
		delete script;
		delete sound;
	}

	void Engine::Startup(Config config)
	{
		this->config = config;
		graphics->Startup(config);
		script->Startup();
		sound->Startup();
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

		sound->Shutdown();
		script->Shutdown();
		graphics->Shutdown();
	}
}

