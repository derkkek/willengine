#pragma once

namespace willengine
{
	class Engine;
	class PhysicsManager
	{
	public:
		PhysicsManager(Engine* engine);
		~PhysicsManager() = default;

		void Update();
	private:
		Engine* engine;
		float worldWidth;
		float worldHeight;
	};
}
