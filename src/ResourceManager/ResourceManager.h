#pragma once
#include <filesystem>
#include <Types.h>

/* TODO: Do The Extensions on the class.*/
namespace willengine
{
	class Engine;

	class ResourceManager
	{
		typedef std::function<void()> UpdateCallback;
	public:
		ResourceManager(Engine* engine);
		~ResourceManager() = default;
		std::string ResolvePath(const std::string& relativePath);
		void SetRootPath(const std::string& rootPath);
	private:
		Engine* engine;
		std::filesystem::path rootPath;
	};

}
