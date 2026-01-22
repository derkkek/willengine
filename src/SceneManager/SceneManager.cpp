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

namespace willengine
{
	willengine::SceneManager::SceneManager(Engine* engine)
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
    void SceneManager::RunScriptStartFunctions()
    {
        auto& sceneScripts = engine->script->BringScripts();
        for (auto& [name, scriptFunc] : sceneScripts)
        {
            engine->script->CallFunction(name, "Start");
        }
    }
    void SceneManager::Startup()
    {
        LoadScripts();
        LoadSounds();
        LoadSprites();
        CreateGameEntititesWComponents("scripts/config/scene_config.lua");
    }
}