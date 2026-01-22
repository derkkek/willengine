#pragma once
#include <vector>
#include <string>

namespace willengine
{
	class Engine;
	class SceneManager
	{
	public:
		SceneManager(Engine* engine);
		~SceneManager() = default;

		void Startup();
		void LoadScripts();
		void LoadSounds();
		void LoadSprites();
		bool CreateGameEntititesWComponents(const std::string& path);

	private:
		Engine* engine;
	};

}
