#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <optional>
#include "../Types.h"

namespace willengine
{
	class Engine;
	class CreateEntityEvent;
	class SaveSceneEvent;

	// Snapshot of a single entity's state (for play/stop functionality)
	struct EntitySnapshot {
		std::string name;
		entityID id;

		// Component data (optional means component may not exist)
		std::optional<Transform> transform;
		std::optional<Rigidbody> rigidbody;
		std::optional<Sprite> sprite;
		std::optional<BoxCollider> boxCollider;
		std::optional<Health> health;
		std::optional<Script> script;
	};

	// Complete scene snapshot
	struct SceneSnapshot {
		std::vector<EntitySnapshot> entities;
		bool isValid = false;
	};

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

		// Play mode snapshot methods
		void SaveSnapshot();      // Call when Play is clicked
		void RestoreSnapshot();   // Call when Stop is clicked
		bool HasSnapshot() const { return playModeSnapshot.isValid; }

	private:
		Engine* engine;
		std::unordered_map<std::string, entityID> namedEntities;

		bool entityListDirty = true;
		SceneSnapshot playModeSnapshot;

		void MarkDirty() { entityListDirty = true; }
	};

}
