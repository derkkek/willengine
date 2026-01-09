#include "ResourceManager.h"
#include <string>

ResourceManager::ResourceManager()
	:rootPath("assets")
{
}

std::string ResourceManager::ResolvePath(std::string& relativePath)
{
	return (rootPath / relativePath).string();
}

void ResourceManager::SetRootPath(std::string& rootPath)
{
	this->rootPath = rootPath;
}
