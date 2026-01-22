#pragma once
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <unordered_map>
#include "../ECS/ECS.h"
namespace willengine
{
	class Engine;
	class ScriptManager
	{
		friend class ResourceManager;
		friend class SceneManager;
	public:
		ScriptManager(Engine* engine);
		~ScriptManager() = default;

		void Startup();
		void Shutdown();

		sol::protected_function* GetScript(const std::string& name);

		void InitializeEntityScript(entityID entity, const std::string& scriptName);

		void CallEntityFunction(entityID entity, const std::string& functionName);

		void UpdateAllEntityScripts();

		void StartAllEntityScripts();

		bool RunScript(const std::string& name);

		// Call a Lua function with no parameters and no return value
		bool CallFunction(const std::string& scriptName, const std::string& functionName);

		// Call a Lua function with parameters and get a return value
		template<typename Ret, typename... Args>
		sol::optional<Ret> CallFunction(const std::string& scriptName, const std::string& functionName, Args&&... args);

		// Call a Lua function with parameters but no return value
		template<typename... Args>
		bool CallFunctionVoid(const std::string& scriptName, const std::string& functionName, Args&&... args);

		const std::unordered_map<std::string, sol::protected_function>& BringScripts() const;

	private:
		sol::state lua;
		Engine* engine;

		std::unordered_map<std::string, sol::protected_function> scripts;
		std::unordered_map<entityID, sol::table> scriptInstances;
	};

}
