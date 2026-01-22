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
            { "SPACE", InputManager::Key::SPACE },
            { "ESC", InputManager::Key::ESC }
            });
        lua["Input"] = input_namespace;

        // Expose the Shutdown function
        lua.set_function("Stop", [this]()
            {
                engine->Stop();
            });



        auto graphics_namespace = lua.create_table();
        graphics_namespace["LoadImage"] = [this](const std::string& name, const std::string& path)
            {
                const std::string& resolvedPath = engine->resource->ResolvePath(path);
                engine->resource->LoadTexture(name, path);
            };

        lua["Graphics"] = graphics_namespace;

        auto resource_namespace = lua.create_table();
        resource_namespace["LoadScript"] = [this](const std::string& name, const std::string& path)
            {
                const std::string& resolvedPath = engine->resource->ResolvePath(path);
                engine->resource->LoadScript(name, resolvedPath);
            };
        lua["Resource"] = resource_namespace;

        auto ecs_namespace = lua.create_table();
        ecs_namespace["CreateEntity"] = [this]()
            {
                return engine->ecs.Create();
            };
        // AddComponent overloads for each component type
        ecs_namespace["AddComponent"] = sol::overload(
            // Transform component
            [this](entityID entity, Transform component)
            {
                engine->ecs.Get<Transform>(entity) = component;
            },
            // Sprite component
            [this](entityID entity, Sprite component)
            {
                engine->ecs.Get<Sprite>(entity) = component;
            },
            // Rigidbody component
            [this](entityID entity, Rigidbody component)
            {
                engine->ecs.Get<Rigidbody>(entity) = component;
            },
            // Velocity component
            [this](entityID entity, Velocity component)
            {
                engine->ecs.Get<Velocity>(entity) = component;
            },
            // Health component
            [this](entityID entity, Health component)
            {
                engine->ecs.Get<Health>(entity) = component;
            },
            // Gravity component
            [this](entityID entity, Gravity component)
            {
                engine->ecs.Get<Gravity>(entity) = component;
            },
            // Script component
            [this](entityID entity, Script component)
            {
                engine->ecs.Get<Script>(entity) = component;
            },
            [this](entityID entity, BoxCollider component)
            {
                engine->ecs.Get<BoxCollider>(entity) = component;
            }
        );

        // GetComponent - returns pointer to component (nil if not found)
        ecs_namespace["GetTransform"] = [this](entityID entity) -> Transform* {
            if (engine->ecs.Has<Transform>(entity)) {
                return &engine->ecs.Get<Transform>(entity);
            }
            return nullptr;
        };
        ecs_namespace["GetSprite"] = [this](entityID entity) -> Sprite* {
            if (engine->ecs.Has<Sprite>(entity)) {
                return &engine->ecs.Get<Sprite>(entity);
            }
            return nullptr;
        };
        ecs_namespace["GetRigidbody"] = [this](entityID entity) -> Rigidbody* {
            if (engine->ecs.Has<Rigidbody>(entity)) {
                return &engine->ecs.Get<Rigidbody>(entity);
            }
            return nullptr;
        };
        ecs_namespace["GetVelocity"] = [this](entityID entity) -> Velocity* {
            if (engine->ecs.Has<Velocity>(entity)) {
                return &engine->ecs.Get<Velocity>(entity);
            }
            return nullptr;
        };
        ecs_namespace["GetHealth"] = [this](entityID entity) -> Health* {
            if (engine->ecs.Has<Health>(entity)) {
                return &engine->ecs.Get<Health>(entity);
            }
            return nullptr;
        };
        ecs_namespace["GetGravity"] = [this](entityID entity) -> Gravity* {
            if (engine->ecs.Has<Gravity>(entity)) {
                return &engine->ecs.Get<Gravity>(entity);
            }
            return nullptr;
        };
        ecs_namespace["GetScript"] = [this](entityID entity) -> Script* {
            if (engine->ecs.Has<Script>(entity)) {
                return &engine->ecs.Get<Script>(entity);
            }
            return nullptr;
        };
        ecs_namespace["GetBoxCollider"] = [this](entityID entity) -> BoxCollider* {
            if (engine->ecs.Has<BoxCollider>(entity)) {
                return &engine->ecs.Get<BoxCollider>(entity);
            }
            return nullptr;
            };


        // HasComponent - check if entity has a component
        ecs_namespace["HasTransform"] = [this](entityID entity) {
            return engine->ecs.Has<Transform>(entity);
        };
        ecs_namespace["HasSprite"] = [this](entityID entity) {
            return engine->ecs.Has<Sprite>(entity);
        };
        ecs_namespace["HasRigidbody"] = [this](entityID entity) {
            return engine->ecs.Has<Rigidbody>(entity);
        };
        ecs_namespace["HasVelocity"] = [this](entityID entity) {
            return engine->ecs.Has<Velocity>(entity);
        };
        ecs_namespace["HasHealth"] = [this](entityID entity) {
            return engine->ecs.Has<Health>(entity);
        };
        ecs_namespace["HasGravity"] = [this](entityID entity) {
            return engine->ecs.Has<Gravity>(entity);
        };
        ecs_namespace["HasScript"] = [this](entityID entity) {
            return engine->ecs.Has<Script>(entity);
        };
        ecs_namespace["HasBoxCollider"] = [this](entityID entity) {
            return engine->ecs.Has<BoxCollider>(entity);
            };

        // RemoveComponent - remove a component from an entity
        ecs_namespace["RemoveTransform"] = [this](entityID entity) {
            engine->ecs.Drop<Transform>(entity);
        };
        ecs_namespace["RemoveSprite"] = [this](entityID entity) {
            engine->ecs.Drop<Sprite>(entity);
        };
        ecs_namespace["RemoveRigidbody"] = [this](entityID entity) {
            engine->ecs.Drop<Rigidbody>(entity);
        };
        ecs_namespace["RemoveVelocity"] = [this](entityID entity) {
            engine->ecs.Drop<Velocity>(entity);
        };
        ecs_namespace["RemoveHealth"] = [this](entityID entity) {
            engine->ecs.Drop<Health>(entity);
        };
        ecs_namespace["RemoveGravity"] = [this](entityID entity) {
            engine->ecs.Drop<Gravity>(entity);
        };
        ecs_namespace["RemoveScript"] = [this](entityID entity) {
            engine->ecs.Drop<Script>(entity);
        };

        // DestroyEntity - destroy an entity and all its components
        ecs_namespace["DestroyEntity"] = [this](entityID entity) {
            engine->ecs.Destroy(entity);
        };

        lua["ECS"] = ecs_namespace;

        auto sound_namespace = lua.create_table();
        sound_namespace["LoadSound"] = [this](const std::string& name, const std::string& path)
            {
                engine->resource->LoadSound(name, path);
            };
        sound_namespace["DeleteSound"] = [this](const std::string& name)
            {
                engine->resource->DeleteSound(name);
            };
        sound_namespace["Play"] = [this](const std::string& name)
            {
                engine->sound->PlaySound(name);
            };
        lua["Sound"] = sound_namespace;



        lua.new_usertype<glm::vec3>("vec3",
            sol::call_constructor, sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
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
            sol::call_constructor, sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float)>(),
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


        // Register component usertypes
        lua.new_usertype<Sprite>("Sprite",
            sol::call_constructor, sol::constructors<Sprite(), Sprite(const std::string&, float, const glm::vec2&)>(),
            "image", &Sprite::image,
            "alpha", &Sprite::alpha,
            "scale", &Sprite::scale);

        lua.new_usertype<Transform>("Transform",
            sol::call_constructor, sol::constructors<Transform(), Transform(const glm::vec2&)>(),
            "x", &Transform::x,
            "y", &Transform::y);

        lua.new_usertype<Rigidbody>("Rigidbody",
            sol::call_constructor, sol::constructors<Rigidbody(), Rigidbody(const glm::vec2&, const glm::vec2&)>(),
            "position", &Rigidbody::position,
            "velocity", &Rigidbody::velocity);

        lua.new_usertype<Velocity>("Velocity",
            sol::call_constructor, sol::constructors<Velocity(), Velocity(const glm::vec2&)>(),
            "x", &Velocity::x,
            "y", &Velocity::y);

        lua.new_usertype<Health>("Health",
            sol::call_constructor, sol::constructors<Health(), Health(double)>(),
            "percent", &Health::percent);

        lua.new_usertype<Gravity>("Gravity",
            sol::call_constructor, sol::constructors<Gravity()>(),
            "meters_per_second", &Gravity::meters_per_second);

        lua.new_usertype<Script>("Script",
            sol::call_constructor, sol::constructors<Script()>(),
            "name", &Script::name);

        lua.new_usertype<BoxCollider>("BoxCollider",
            sol::call_constructor, sol::constructors<BoxCollider(), BoxCollider(const glm::vec2&, bool)>(),
            "dimensionSizes", &BoxCollider::dimensionSizes,
            "isCollided", &BoxCollider::isCollided);

	}

    sol::protected_function* ScriptManager::GetScript(const std::string& name)
    {
        auto it = scripts.find(name);
        if (it != scripts.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    void ScriptManager::InitializeEntityScript(entityID entity, const std::string& scriptName) {
        // Create a unique table for this entity's script instance
        sol::table instance = lua.create_table();
        instance["entity"] = entity;  // Script can access its own entity

        // Store the instance
        scriptInstances[entity] = instance;

        // Load and run the script file to define functions (if not already loaded)
        std::string scriptPath = engine->resource->ResolvePath("scripts/" + scriptName + ".lua");
        lua.script_file(scriptPath);
    }

    void ScriptManager::CallEntityFunction(entityID entity, const std::string& functionName) {
        auto it = scriptInstances.find(entity);
        if (it == scriptInstances.end()) return;

        sol::table& instance = it->second;

        // Get the function from global state
        sol::optional<sol::protected_function> func = lua[functionName];
        if (func) {
            sol::protected_function_result result = (*func)(instance);
            if (!result.valid()) {
                sol::error err = result;
                spdlog::error("Error in {} for entity {}: {}", functionName, entity, err.what());
            }
        }
    }

    void ScriptManager::UpdateAllEntityScripts() {
        engine->ecs.ForEach<Script>([this](entityID entity) {
            CallEntityFunction(entity, "update");
            });
    }

    void ScriptManager::StartAllEntityScripts() {
        engine->ecs.ForEach<Script>([this](entityID entity) {
            CallEntityFunction(entity, "start");
            });
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

    const std::unordered_map<std::string, sol::protected_function>& ScriptManager::BringScripts() const
    {
        return scripts;
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
