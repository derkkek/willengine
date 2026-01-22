#include "Engine.h"
#include "GraphicsManager/GraphicsManager.h"
#include "InputManager/InputManager.h"
#include "ResourceManager/ResourceManager.h"
#include "ScriptManager/ScriptManager.h"
#include "SoundManager/SoundManager.h"
#include "PhysicsManager/PhysicsManager.h"
#include "SceneManager/SceneManager.h"
#include <iostream>

namespace willengine
{
	Engine::Engine(Config config)
		: config(config),
		  graphics(new GraphicsManager(this)),
		  physics(new PhysicsManager(this)),
		  scene(new SceneManager(this)),
		  input(new InputManager(this)),
		  resource(new ResourceManager(this)),
		  script(new ScriptManager(this)),
		  event(new EventManager),
	      sound(new SoundManager(this)),
		  running(false)
	{
		Startup(config);
	}

	Engine::~Engine()
	{
		delete graphics;
		delete physics;
		delete input;
		delete resource;
		delete script;
		delete sound;
		delete scene;
	}

	void Engine::Startup(Config config)
	{
		graphics->Startup(this->config);
		physics->Startup(this->config);
		script->Startup();
		sound->Startup();
		scene->Startup();
		running = true;
	}

	void Engine::Stop()
	{
		running = false;
	}

	void Engine::RunGameLoop(const UpdateCallback& callback)
	{

					
		double now = glfwGetTime();
		double timePerExecution = 1.0 / 60.0;
		double lastTick = now - timePerExecution;

		script->StartAllEntityScripts();

		while (running && !graphics->ShouldQuit())
		{
			now = glfwGetTime();
			while (now >= lastTick + timePerExecution)
			{
				input->Update();
				script->UpdateAllEntityScripts();
				callback();
				lastTick += timePerExecution;
				physics->Update();
			}

			graphics->Draw();

		}
	}

	void Engine::Shutdown()
	{
		sound->Shutdown();
		script->Shutdown();
		graphics->Shutdown();
	}

	Engine::Config& Engine::BringEngineConfiguration()
	{
		return config;
	}
}

