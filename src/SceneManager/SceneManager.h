#pragma once
#include <vector>
#include <string>

namespace willengine
{
	class Engine;
	class CreateEntityEvent;
	class SaveEntityToConfigFileEvent;
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

		void SubscribeToEvents();
		void OnCreateEntity(CreateEntityEvent& event);
		void OnSaveEntityToConfig(SaveEntityToConfigFileEvent& event);


	private:
		Engine* engine;
	};

}
