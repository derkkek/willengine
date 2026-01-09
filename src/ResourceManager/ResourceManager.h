#pragma once
#include <filesystem>
#include <Types.h>

/* TODO: Do The Extensions on the class.*/

class ResourceManager
{
	typedef std::function<void()> UpdateCallback;
public:
	ResourceManager();
	~ResourceManager() = default;
	std::string ResolvePath(std::string& relativePath);
	void SetRootPath(std::string& rootPath);
private:
	std::filesystem::path rootPath;
};
