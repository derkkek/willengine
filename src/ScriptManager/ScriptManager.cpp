#include "ScriptManager.h"
#include "../Engine.h"
#include "../ResourceManager/ResourceManager.h"
#include "../InputManager/InputManager.h"
#include "../GraphicsManager/GraphicsManager.h"
#include "../SoundManager/SoundManager.h"
#include <spdlog/spdlog.h>
#include <unordered_set>
namespace willengine
{
	ScriptManager::ScriptManager(Engine* engine) :engine(engine)
	{
	}

	void ScriptManager::Startup()
	{
		lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table,sol::lib::os, sol::lib::string, sol::lib::io, sol::lib::debug);

        // Load the debugger
        /*
            dbg()  -- This sets a breakpoint
            print("This line will be where execution pauses")
        */
        std::string debuggerPath = engine->resource->ResolvePath("scripts/debugger.lua");
        lua.require_file("dbg", debuggerPath);



        /* 
            Lua namespaces organize lua scripting syntax as following; 
            Graphics.LoadTexture("player.png")
            Input.KeyIsDown(KEYBOARD.A)
            Instead of global LoadTexture() or KeyIsDown()
            
            like in c++;

            // C++ without namespace - messy
            LoadTexture();
            KeyIsDown();

            // C++ with namespace - organized
            graphics::LoadTexture();
            input::KeyIsDown();
        */

        auto input_namespace = lua.create_table();

        // Expose the KeyIsPressed function to Lua
        input_namespace["KeyHoldingDown"] = [this](int keycode) 
            {
            return engine->input->KeyIsPressedInFrame(static_cast<InputManager::Key>(keycode));
            };
        input_namespace["KeyReleased"] = [this](int keycode)
            {
                return engine->input->KeyJustReleased((static_cast<InputManager::Key>(keycode)));
            };
        input_namespace["KeyJustPressed"] = [this](int keycode)
            {
                return engine->input->KeyJustPressed((static_cast<InputManager::Key>(keycode)));
            };

        // Expose keyboard constants to Lua
        lua.new_enum<int>("KEYBOARD", {
            { "W", InputManager::Key::W },
            { "A", InputManager::Key::A },
            { "S", InputManager::Key::S },
            { "D", InputManager::Key::D },
            { "ESC", InputManager::Key::ESC }
            });
        lua["Input"] = input_namespace;

        // Expose the Shutdown function
        lua.set_function("Shutdown", [this]()
            {
                engine->Shutdown();
            });



        auto graphics_namespace = lua.create_table();
        graphics_namespace["LoadImage"] = [this](const std::string& name, const std::string& path)
            {
                const std::string& resolvedPath = engine->resource->ResolvePath(path);
                engine->graphics->LoadTexture(name, path);
            };

        graphics_namespace["AddSprite"] = [this](const std::string& name, const std::string& path, glm::vec3 postion = glm::vec3(0,0,1), glm::vec2 scale = glm::vec2(20,20))
            {
                engine->graphics->AddSprite(name, postion, scale, path);
            };
        lua["Graphics"] = graphics_namespace;

        auto resource_namespace = lua.create_table();
        resource_namespace["LoadScript"] = [this](const std::string& name, const std::string& path)
            {
                const std::string& resolvedPath = engine->resource->ResolvePath(path);
                LoadScript(name, resolvedPath);
            };
        lua["Resource"] = resource_namespace;

        auto ecs_namespace = lua.create_table();
        ecs_namespace["CreateEntity"] = [this]()
            {
                engine->ecs.Create();
            };
        lua["ECS"] = resource_namespace;

        auto sound_namespace = lua.create_table();
        sound_namespace["LoadSound"] = [this](const std::string& name, const std::string& path)
            {
                engine->sound->LoadSound(name, path);
            };
        sound_namespace["DeleteSound"] = [this](const std::string& name)
            {
                engine->sound->DeleteSound(name);
            };
        sound_namespace["Play"] = [this](const std::string& name)
            {
                engine->sound->PlaySound(name);
            };
        lua["Sound"] = sound_namespace;

        lua.new_usertype<Sprite>("Sprite",
            sol::constructors<Sprite()>(),
            "image", &Sprite::image,
            "position", &Sprite::position,
            "scale", &Sprite::scale);

        lua.new_usertype<glm::vec3>("vec3",
            sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z,
            // optional and fancy: operator overloading. see: https://github.com/ThePhD/sol2/issues/547
            sol::meta_function::addition, sol::overload([](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1 + v2; }),
            sol::meta_function::subtraction, sol::overload([](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1 - v2; }),
            sol::meta_function::multiplication, sol::overload(
                [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1 * v2; },
                [](const glm::vec3& v1, float f) -> glm::vec3 { return v1 * f; },
                [](float f, const glm::vec3& v1) -> glm::vec3 { return f * v1; }
            )
        );

        lua.new_usertype<glm::vec2>("vec2",
            sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>(),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y,
            // optional and fancy: operator overloading. see: https://github.com/ThePhD/sol2/issues/547
            sol::meta_function::addition, sol::overload([](const glm::vec2& v1, const glm::vec2& v2) -> glm::vec2 { return v1 + v2; }),
            sol::meta_function::subtraction, sol::overload([](const glm::vec2& v1, const glm::vec2& v2) -> glm::vec2 { return v1 - v2; }),
            sol::meta_function::multiplication, sol::overload(
                [](const glm::vec2& v1, const glm::vec2& v2) -> glm::vec2 { return v1 * v2; },
                [](const glm::vec2& v1, float f) -> glm::vec2 { return v1 * f; },
                [](float f, const glm::vec2& v1) -> glm::vec2 { return f * v1; }
            )
        );
	}

	bool ScriptManager::LoadScript(const std::string& name, const std::string& path)
	{
        std::string resolvedPath = engine->resource->ResolvePath(path);
        sol::load_result loadResult = lua.load_file(resolvedPath);

        if (!loadResult.valid()) {
            sol::error err = loadResult;
            spdlog::error("Failed to load script '{}': {}", name, err.what());
            return false;
        }

        // Convert load_result to protected_function and store it
        sol::protected_function script = loadResult;
        scripts[name] = script;

        spdlog::info("Loaded script '{}'", name);
        return true;
	}

    sol::protected_function* ScriptManager::GetScript(const std::string& name)
    {
        auto it = scripts.find(name);
        if (it != scripts.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    bool ScriptManager::RunScript(const std::string& name)
    {
        sol::protected_function* script = GetScript(name);
        if (!script) {
            spdlog::error("Script '{}' not found", name);
            return false;
        }

        sol::protected_function_result result = (*script)();
        if (!result.valid()) {
            sol::error err = result;
            spdlog::error("Error running script '{}': {}", name, err.what());
            return false;
        }

        return true;
    }

    bool ScriptManager::CallFunction(const std::string& scriptName, const std::string& functionName)
    {
        // First, make sure the script is loaded (run it once to define functions)
        sol::protected_function* script = GetScript(scriptName);
        if (!script) {
            spdlog::error("Script '{}' not found", scriptName);
            return false;
        }

        // Execute script once if not already executed (to define functions)
        static std::unordered_set<std::string> executed_scripts;
        if (executed_scripts.find(scriptName) == executed_scripts.end()) {
            sol::protected_function_result result = (*script)();
            if (!result.valid()) {
                sol::error err = result;
                spdlog::error("Error initializing script '{}': {}", scriptName, err.what());
                return false;
            }
            executed_scripts.insert(scriptName);
        }

        // Get the function from Lua global state
        sol::optional<sol::protected_function> func = lua[functionName];
        if (!func) {
            spdlog::error("Function '{}' not found in script '{}'", functionName, scriptName);
            return false;
        }

        // Call the function
        sol::protected_function_result result = (*func)();
        if (!result.valid()) {
            sol::error err = result;
            spdlog::error("Error calling function '{}' in script '{}': {}", functionName, scriptName, err.what());
            return false;
        }

        return true;
    }

    template<typename Ret, typename... Args>
    sol::optional<Ret> ScriptManager::CallFunction(const std::string& scriptName, const std::string& functionName, Args&&... args)
    {
        sol::protected_function* script = GetScript(scriptName);
        if (!script) {
            spdlog::error("Script '{}' not found", scriptName);
            return sol::nullopt;
        }

        // Execute script once to define functions
        static std::unordered_set<std::string> executed_scripts;
        if (executed_scripts.find(scriptName) == executed_scripts.end()) {
            sol::protected_function_result result = (*script)();
            if (!result.valid()) {
                sol::error err = result;
                spdlog::error("Error initializing script '{}': {}", scriptName, err.what());
                return sol::nullopt;
            }
            executed_scripts.insert(scriptName);
        }

        sol::optional<sol::protected_function> func = lua[functionName];
        if (!func) {
            spdlog::error("Function '{}' not found in script '{}'", functionName, scriptName);
            return sol::nullopt;
        }

        sol::protected_function_result result = (*func)(std::forward<Args>(args)...);
        if (!result.valid()) {
            sol::error err = result;
            spdlog::error("Error calling function '{}': {}", functionName, err.what());
            return sol::nullopt;
        }

        return result.get<Ret>();
    }

    template<typename... Args>
    bool ScriptManager::CallFunctionVoid(const std::string& scriptName, const std::string& functionName, Args&&... args)
    {
        sol::protected_function* script = GetScript(scriptName);
        if (!script) {
            spdlog::error("Script '{}' not found", scriptName);
            return false;
        }

        static std::unordered_set<std::string> executed_scripts;
        if (executed_scripts.find(scriptName) == executed_scripts.end()) {
            sol::protected_function_result result = (*script)();
            if (!result.valid()) {
                sol::error err = result;
                spdlog::error("Error initializing script '{}': {}", scriptName, err.what());
                return false;
            }
            executed_scripts.insert(scriptName);
        }

        sol::optional<sol::protected_function> func = lua[functionName];
        if (!func) {
            spdlog::error("Function '{}' not found in script '{}'", functionName, scriptName);
            return false;
        }

        sol::protected_function_result result = (*func)(std::forward<Args>(args)...);
        if (!result.valid()) {
            sol::error err = result;
            spdlog::error("Error calling function '{}': {}", functionName, err.what());
            return false;
        }

        return true;
    }

    void ScriptManager::Shutdown()
    {

    }
}
