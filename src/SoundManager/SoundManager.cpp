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