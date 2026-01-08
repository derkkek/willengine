#pragma once
#include "../Engine.h"

namespace willengine
{
	class Engine;
	struct Config;


	class GraphicsManager
	{
	public:
		GraphicsManager(Engine* engine);
		~GraphicsManager();

		void Startup(Engine::Config config);
		void Shutdown();
		void Draw();

	private:
		Engine* engine;
	};
}
