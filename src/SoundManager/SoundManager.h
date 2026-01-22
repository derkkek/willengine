#pragma once
#include <soloud.h>
#include <soloud_wav.h>
#include <string>
#include <unordered_map>

namespace willengine
{
	class Engine;
	class SoundManager
	{
		friend class ResourceManager;
	public:
		SoundManager(Engine* engine);
		~SoundManager();
		void Startup();
		void Shutdown();
		void PlaySound(const std::string& name);
	private:
		Engine* engine;
		SoLoud::Soloud soloud;
		SoLoud::Wav wav;

		std::unordered_map<std::string, SoLoud::Wav> nameToSoundMap;
	};
}
