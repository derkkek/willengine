#pragma once
#include "../Engine.h"
namespace willengine
{
	class Engine;
	class PhysicsManager
	{
	public:
		PhysicsManager(Engine* engine);
		~PhysicsManager() = default;

		void Startup(Engine::Config& config);
		void Update();
	private:
		Engine* engine;
		float worldHalfHeight;
		float worldHalfWidth;
	};
}
