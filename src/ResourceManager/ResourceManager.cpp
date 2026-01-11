#include "ResourceManager.h"
#include <string>
#include "../Engine.h"
namespace willengine
{
	ResourceManager::ResourceManager(Engine* engine)
		:rootPath("assets"), engine(engine)
	{
	}

	std::string ResourceManager::ResolvePath(const std::string& relativePath)
	{
		return (rootPath / relativePath).string();
	}

	void ResourceManager::SetRootPath(const std::string& rootPath)
	{
		this->rootPath = rootPath;
	}

}
