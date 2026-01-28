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

		void ModifyTransform(const std::string& entityName, float x, float y);
		void ModifyRigidbody(const std::string& entityName, float posX, float posY, float velX, float velY);
		void ModifySprite(const std::string& entityName, const std::string& image, float alpha, float scaleX, float scaleY);
		void ModifyBoxCollider(const std::string& entityName, float width, float height);
		void ModifyHealth(const std::string& entityName, float amount);
		void ModifyScript(const std::string& entityName, const std::string& scriptName);

		const std::unordered_map<std::string, entityID>& GetNamedEntities() const {
			return namedEntities;
		}

		bool IsDirty() const { return entityListDirty; }
		void ClearDirty() { entityListDirty = false; }


	private:
		Engine* engine;
		std::unordered_map<std::string, entityID> namedEntities;

		bool entityListDirty = true;

		void MarkDirty() { entityListDirty = true; }
	};

}
