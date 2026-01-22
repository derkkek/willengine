#pragma once
#include <filesystem>
#include <Types.h>

/* TODO: Do The Extensions on the class.*/
namespace willengine
{
	class Engine;

	class ResourceManager
	{
		//typedef std::function<void()> UpdateCallback; ??
	public:
		ResourceManager(Engine* engine);
		~ResourceManager() = default;
		std::string ResolvePath(const std::string& relativePath);
		void SetRootPath(const std::string& rootPath);

		bool LoadSound(const std::string& name, const std::string& relativePath);
		bool DeleteSound(const std::string& name);

		bool LoadScript(const std::string& name, const std::string& relativePath);
		bool DeleteScript(const std::string& name);

		bool LoadTexture(const std::string& name, const std::string& relativePath);
		bool DeleteTexture(const std::string& name);
	private:
		Engine* engine;
		std::filesystem::path rootPath;
	};

}
