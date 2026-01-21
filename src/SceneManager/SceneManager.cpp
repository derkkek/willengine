#include "SceneManager.h"
#include "../Engine.h"
#include "../ScriptManager/ScriptManager.h"
#include "../ResourceManager/ResourceManager.h"
#include "../SoundManager/SoundManager.h"
#include "../GraphicsManager/GraphicsManager.h"
#include <spdlog/spdlog.h>

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

                if (engine->script->LoadScript(name, assetRelative.string())) 
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

                if (engine->sound->LoadSound(name, assetRelative.string()))
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

                if (engine->graphics->LoadTexture(name, assetRelative.string()))
                {
                    spdlog::info("Auto-loaded sprite: {}", name);
                }
            }
        }
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
        RunScriptStartFunctions();
    }
}