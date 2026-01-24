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
#include "../Events/SaveEntityToConfigFileEvent.h"
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

    void SceneManager::OnSaveEntityToConfig(SaveEntityToConfigFileEvent& event)
    {
        const EntitySaveData& data = event.saveData;

        std::string configPath = engine->resource->ResolvePath("scripts/config/scene_config.lua");

        // Read existing file
        std::ifstream inFile(configPath);
        std::stringstream buffer;
        buffer << inFile.rdbuf();
        std::string content = buffer.str();
        inFile.close();

        // Find the position to insert (before the last closing braces of entities table)
        // Look for the pattern "    }\n\n    }" or similar at the end
        size_t insertPos = content.rfind("\n    }");  // Find last "    }" (end of entities)

        if (insertPos == std::string::npos) {
            spdlog::error("Could not find insertion point in scene_config.lua");
            return;
        }

        // Build new entity Lua string
        std::stringstream entityLua;
        entityLua << ",\n";  // comma after previous entity
        entityLua << "        {\n";
        entityLua << "            id = {entityID = \"" << data.entityID << "\"},\n";
        entityLua << "\n";
        entityLua << "            components = {\n";

        if (data.transform.has_value()) {
            entityLua << "                transform = {x = " << data.transform->x
                << " , y = " << data.transform->y << "},\n";
        }

        if (data.rigidbody.has_value()) {
            entityLua << "                rigidbody = {x = " << data.rigidbody->position.x
                << ", y = " << data.rigidbody->position.y
                << ", x_vel = " << data.rigidbody->velocity.x
                << ", y_vel = " << data.rigidbody->velocity.y << "},\n";
        }

        if (data.sprite.has_value()) {
            entityLua << "                sprite = {sprite_id = \"" << data.sprite->image
                << "\", sprite_alpha = " << data.sprite->alpha
                << ", width = " << data.sprite->scale.x
                << ", height = " << data.sprite->scale.y << "},\n";
        }

        if (data.boxCollider.has_value()) {
            entityLua << "                box_collider = {width = " << data.boxCollider->dimensionSizes.x
                << ", height = " << data.boxCollider->dimensionSizes.y
                << ", isCollided = false},\n";
        }

        if (data.health.has_value()) {
            entityLua << "                health = {amount = " << data.health->percent << "},\n";
        }

        if (data.script.has_value() && !data.script->name.empty()) {
            entityLua << "                script = {name = \"" << data.script->name << "\"}\n";
        }

        entityLua << "            }\n";
        entityLua << "        }";

        // Insert the new entity
        content.insert(insertPos, entityLua.str());

        // Write back to file
        std::ofstream outFile(configPath);
        if (!outFile.is_open()) {
            spdlog::error("Failed to open scene_config.lua for writing");
            return;
        }
        outFile << content;
        outFile.close();

        spdlog::info("Saved entity '{}' to scene_config.lua", data.entityID);
    }

    void SceneManager::SubscribeToEvents()
    {
        engine->event->SubscribeToEvent<CreateEntityEvent>(this, &SceneManager::OnCreateEntity);
        engine->event->SubscribeToEvent<SaveEntityToConfigFileEvent>(this, &SceneManager::OnSaveEntityToConfig);
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