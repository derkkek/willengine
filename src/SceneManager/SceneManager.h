#pragma once
#include <vector>
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
		void RunScriptStartFunctions();

	private:
		Engine* engine;
	};

}
