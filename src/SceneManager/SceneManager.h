#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "../Types.h"

namespace willengine
{
	class Engine;
	class CreateEntityEvent;
	class SaveSceneEvent;
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
		void OnSaveScene(SaveSceneEvent& event);


	private:
		Engine* engine;
		std::unordered_map<std::string, entityID> namedEntities;
	};

}
