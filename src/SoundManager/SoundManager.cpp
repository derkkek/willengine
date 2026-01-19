#include "SoundManager/SoundManager.h"
#include "../Engine.h"
#include "../ResourceManager/ResourceManager.h"
#include "spdlog/spdlog.h"

namespace willengine
{
	SoundManager::SoundManager(Engine* engine) : engine(engine) {}
	SoundManager::~SoundManager() {}

	void SoundManager::Startup()
	{
		soloud.init();
		wav.createInstance();
	}

	void SoundManager::Shutdown()
	{
		soloud.deinit();
	}
	bool SoundManager::LoadSound(const std::string& name, const std::string& path)
	{
		const std::string resolvedPath = engine->resource->ResolvePath(path);
		if (!resolvedPath.empty())
		{
			nameToSoundMap[name].load(resolvedPath.c_str());
			spdlog::info("sound: " + name + " has loaded");
			return true;
		}
		return false;
	}
	bool SoundManager::DeleteSound(const std::string& name)
	{
		if (nameToSoundMap.contains(name))
		{
			nameToSoundMap.erase(name);
			return true;
		}
		spdlog::error("following sound isn't included in the sounds: " + name);
		return false;
	}
	void SoundManager::PlaySound(const std::string& name)
	{
		if (nameToSoundMap.contains(name))
		{
			soloud.play(nameToSoundMap[name]);
		}
		else
		{
			spdlog::error("Sound not found: " + name);
		}
	}
}