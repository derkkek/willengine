#include "SceneManager.h"
#include "../Engine.h"
#include "../ScriptManager/ScriptManager.h"
#include "../ResourceManager/ResourceManager.h"
#include "../SoundManager/SoundManager.h"
#include "../GraphicsManager/GraphicsManager.h"
#include <spdlog/spdlog.h>
#include <filesystem>          
#include "../Types.h"          
#include "../ECS/ECS.h"    
#include "../Events/CreateEntityEvent.h"
#include "../Events/SaveSceneEvent.h"
#include <fstream>

namespace willengine
{
	SceneManager::SceneManager(Engine* engine)
		:engine(engine)
	{
	}

    void SceneManager::LoadScripts()
    {
        std::string scriptsDir = engine->resource->ResolvePath("scripts");
        std::filesystem::path scriptsPath(scriptsDir);

        if (!std::filesystem::exists(scriptsPath)) {
            spdlog::warn("Scripts directory not found: {}", scriptsDir);
            return;
        }

        // Use recursive_directory_iterator for subdirectories
        for (const auto& entry : std::filesystem::recursive_directory_iterator(scriptsPath)) 
        {
            if (entry.is_regular_file() && entry.path().extension() == ".lua") 
            {
                // Get path relative to scripts folder for the name
                std::filesystem::path relativePath = std::filesystem::relative(entry.path(), scriptsPath);
                std::string name = relativePath.replace_extension("").string();  // e.g., "enemies/goblin"

                // Get path relative to assets folder for loading
                std::filesystem::path assetRelative = std::filesystem::relative(entry.path(),
                    engine->resource->ResolvePath(""));

                if (engine->resource->LoadScript(name, assetRelative.string())) 
                {
                    spdlog::info("Auto-loaded script: {}", name);
                }
            }
        }
    }
    void SceneManager::LoadSounds()
    {
        std::string soundsDir = engine->resource->ResolvePath("sounds");
        std::filesystem::path soundsPath(soundsDir);

        if (!std::filesystem::exists(soundsPath)) {
            spdlog::warn("Sounds directory not found: {}", soundsDir);
            return;
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(soundsDir))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".wav")
            {
                // Get path relative to scripts folder for the name
                std::filesystem::path relativePath = std::filesystem::relative(entry.path(), soundsPath);
                std::string name = relativePath.replace_extension("").string();

                // Get path relative to assets folder for loading
                std::filesystem::path assetRelative = std::filesystem::relative(entry.path(),
                    engine->resource->ResolvePath(""));

                if (engine->resource->LoadSound(name, assetRelative.string()))
                {
                    spdlog::info("Auto-loaded sound: {}", name);
                }
            }
        }
    }
    void SceneManager::LoadSprites()
    {
        std::string spritesDir = engine->resource->ResolvePath("sprites");
        std::filesystem::path spritesPath(spritesDir);

        if (!std::filesystem::exists(spritesPath)) {
            spdlog::warn("Sprites directory not found: {}", spritesDir);
            return;
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(spritesDir))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".png")
            {
                // Get path relative to scripts folder for the name
                std::filesystem::path relativePath = std::filesystem::relative(entry.path(), spritesPath);
                std::string name = relativePath.replace_extension("").string();

                // Get path relative to assets folder for loading
                std::filesystem::path assetRelative = std::filesystem::relative(entry.path(),
                    engine->resource->ResolvePath(""));

                if (engine->resource->LoadTexture(name, assetRelative.string()))
                {
                    spdlog::info("Auto-loaded sprite: {}", name);
                }
            }
        }
    }
    bool SceneManager::CreateGameEntititesWComponents(const std::string& path)
    {
        std::string resolvedPath = engine->resource->ResolvePath(path);

        // Execute the scene file to get the Scene table
        sol::protected_function_result result = engine->script->lua.script_file(resolvedPath);
        if (!result.valid()) {
            sol::error err = result;
            spdlog::error("Failed to load scene '{}': {}", path, err.what());
            return false;
        }

        // Get the Scene table
        sol::optional<sol::table> sceneOpt = engine->script->lua["Scene"];
        if (!sceneOpt) {
            spdlog::error("Scene table not found in '{}'", path);
            return false;
        }
        sol::table scene = *sceneOpt;

        // Get entities table
        sol::optional<sol::table> entitiesOpt = scene["entities"];
        if (!entitiesOpt) {
            spdlog::error("No entities table in scene");
            return false;
        }
        sol::table entities = *entitiesOpt;

        // Iterate over all entities
        for (auto& [key, value] : entities) {
            sol::table entityDef = value.as<sol::table>();
            entityID entity = engine->ecs.Create();

            // Register the entity by its config name as a Lua global
            sol::optional<sol::table> idOpt = entityDef["id"];
            if (idOpt) {
                std::string entityName = idOpt->get_or<std::string>("entityID", "");
                if (!entityName.empty()) {
                    engine->script->lua[entityName] = entity;  // Makes "player" available in Lua
                    namedEntities[entityName] = entity;
                    spdlog::info("Registered entity '{}' with ID {}", entityName, entity);
                }
            }

            sol::optional<sol::table> componentsOpt = entityDef["components"];
            if (!componentsOpt) continue;
            sol::table components = *componentsOpt;

            // Transform
            if (sol::optional<sol::table> t = components["transform"]) {
                Transform& transform = engine->ecs.Get<Transform>(entity);
                transform.x = t->get_or("x", 0.0f);
                transform.y = t->get_or("y", 0.0f);
            }

            // Rigidbody
            if (sol::optional<sol::table> rb = components["rigidbody"]) {
                Rigidbody& rigidbody = engine->ecs.Get<Rigidbody>(entity);
                rigidbody.position = glm::vec2(rb->get_or("x", 0.0f), rb->get_or("y", 0.0f));
                rigidbody.velocity = glm::vec2(rb->get_or("x_vel", 0.0f), rb->get_or("y_vel", 0.0f));
            }
            // Script
            if (sol::optional<sol::table> scriptDef = components["script"]) {
                std::string scriptName = scriptDef->get_or<std::string>("name", "");
                if (!scriptName.empty()) {
                    Script& script = engine->ecs.Get<Script>(entity);
                    script.name = scriptName;

                    // Create a Lua table instance for this entity's script state
                    engine->script->InitializeEntityScript(entity, scriptName);

                    spdlog::info("Attached script '{}' to entity", scriptName);
                }
            }

            // Sprite
            if (sol::optional<sol::table> s = components["sprite"]) {
                Sprite& sprite = engine->ecs.Get<Sprite>(entity);
                sprite.image = s->get_or<std::string>("sprite_id", "");
                sprite.alpha = s->get_or("sprite_alpha", 1.0f);
                sprite.scale = glm::vec2(s->get_or("width", 1.0f), s->get_or("height", 1.0f));
            }

            // BoxCollider
            if (sol::optional<sol::table> bc = components["box_collider"]) {
                BoxCollider& collider = engine->ecs.Get<BoxCollider>(entity);
                collider.dimensionSizes = glm::vec2(bc->get_or("width", 1.0f), bc->get_or("height", 1.0f));
                collider.isCollided = bc->get_or("isCollided", false);
            }

            // Health
            if (sol::optional<sol::table> h = components["health"]) {
                Health& health = engine->ecs.Get<Health>(entity);
                health.percent = h->get_or("amount", 100.0);
            }

            spdlog::info("Created entity from scene config");
        }

        return true;
    }

    void SceneManager::OnCreateEntity(CreateEntityEvent& event)
    {
        const EntityCreationData& data = event.entityData;

        // Create the entity
        entityID entity = engine->ecs.Create();

        // Register entity name in Lua if provided
        if (!data.entityID.empty()) {
            engine->script->lua[data.entityID] = entity;
            namedEntities[data.entityID] = entity;
            spdlog::info("Created entity '{}' with ID {}", data.entityID, entity);
        }

        // Add Transform component
        if (data.transform.has_value()) {
            Transform& transform = engine->ecs.Get<Transform>(entity);
            transform.x = data.transform->x;
            transform.y = data.transform->y;
        }

        // Add Rigidbody component
        if (data.rigidbody.has_value()) {
            Rigidbody& rb = engine->ecs.Get<Rigidbody>(entity);
            rb.position = data.rigidbody->position;
            rb.velocity = data.rigidbody->velocity;
        }

        // Add Sprite component
        if (data.sprite.has_value()) {
            Sprite& sprite = engine->ecs.Get<Sprite>(entity);
            sprite.image = data.sprite->image;
            sprite.alpha = data.sprite->alpha;
            sprite.scale = data.sprite->scale;
        }

        // Add BoxCollider component
        if (data.boxCollider.has_value()) {
            BoxCollider& collider = engine->ecs.Get<BoxCollider>(entity);
            collider.dimensionSizes = data.boxCollider->dimensionSizes;
            collider.isCollided = false;
        }

        // Add Health component
        if (data.health.has_value()) {
            Health& health = engine->ecs.Get<Health>(entity);
            health.percent = data.health.value().percent;
        }

        // Add Script component
        if (data.script.has_value()) {
            Script& script = engine->ecs.Get<Script>(entity);
            script.name = data.script.value().name;
            engine->script->InitializeEntityScript(entity, script.name);
            spdlog::info("Attached script '{}' to entity", script.name);
        }

        spdlog::info("Entity created via CreateEntityEvent");
    }

    void SceneManager::OnSaveScene(SaveSceneEvent& event)
    {
        // 1. Open the file (this OVERWRITES the existing file)
        std::string resolvedPath = engine->resource->ResolvePath("scripts/config/scene_config.lua");
        std::ofstream file(resolvedPath);

        if (!file.is_open()) {
            spdlog::error("Failed to open scene_config.lua for writing");
            return;
        }

        // 2. Write the opening structure
        file << "Scene = {\n";
        file << "    entities = {\n";

        // 3. Loop through EVERY entity currently in your ECS
        bool firstEntity = true;
        for (auto& [name, entityId] : namedEntities) {

            // Add comma between entities (but not before the first one)
            if (!firstEntity) {
                file << ",\n";
            }
            firstEntity = false;

            // 4. Write this entity's data
            file << "        {\n";
            file << "            id = {entityID = \"" << name << "\"},\n";
            file << "\n";
            file << "            components = {\n";

            // Track if we need commas between components
            bool firstComponent = true;

            if (engine->ecs.Has<Transform>(entityId)) {
                if (!firstComponent) file << ",\n";
                firstComponent = false;

                Transform& t = engine->ecs.Get<Transform>(entityId);
                file << "                transform = {x = " << t.x << " , y = " << t.y << "}";
            }

            if (engine->ecs.Has<Rigidbody>(entityId)) {
                if (!firstComponent) file << ",\n";
                firstComponent = false;

                Rigidbody& rb = engine->ecs.Get<Rigidbody>(entityId);
                file << "                rigidbody = {x = " << rb.position.x
                    << ", y = " << rb.position.y
                    << ", x_vel = " << rb.velocity.x
                    << ", y_vel = " << rb.velocity.y << "}";
            }

            if (engine->ecs.Has<Sprite>(entityId)) {
                if (!firstComponent) file << ",\n";
                firstComponent = false;

                Sprite& s = engine->ecs.Get<Sprite>(entityId);
                file << "                sprite = {sprite_id = \"" << s.image
                    << "\", sprite_alpha = " << s.alpha
                    << ", width = " << s.scale.x
                    << ", height = " << s.scale.y << "}";
            }

            if (engine->ecs.Has<BoxCollider>(entityId)) {
                if (!firstComponent) file << ",\n";
                firstComponent = false;

                BoxCollider& bc = engine->ecs.Get<BoxCollider>(entityId);
                file << "                box_collider = {width = " << bc.dimensionSizes.x
                    << ", height = " << bc.dimensionSizes.y
                    << ", isCollided = false}";
            }

            if (engine->ecs.Has<Health>(entityId)) {
                if (!firstComponent) file << ",\n";
                firstComponent = false;

                Health& h = engine->ecs.Get<Health>(entityId);
                file << "                health = {amount = " << h.percent << "}";
            }

            if (engine->ecs.Has<Script>(entityId)) {
                if (!firstComponent) file << ",\n";
                firstComponent = false;

                Script& script = engine->ecs.Get<Script>(entityId);
                file << "                script = {name = \"" << script.name << "\"}";
            }

            file << "\n";
            file << "            }\n";
            file << "        }";
        }

        file << "\n\n    }\n";
        file << "}";

        spdlog::info("Scene saved to {}", resolvedPath);
    }

    void SceneManager::SubscribeToEvents()
    {
        engine->event->SubscribeToEvent<CreateEntityEvent>(this, &SceneManager::OnCreateEntity);
        engine->event->SubscribeToEvent<SaveSceneEvent>(this, &SceneManager::OnSaveScene);
    }


    void SceneManager::Startup()
    {
        SubscribeToEvents();
        LoadScripts();
        LoadSounds();
        LoadSprites();
        CreateGameEntititesWComponents("scripts/config/scene_config.lua");
    }
}