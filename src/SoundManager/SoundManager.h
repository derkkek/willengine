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
	public:
		SoundManager(Engine* engine);
		~SoundManager();
		void Startup();
		void Shutdown();
		bool LoadSound(const std::string& name, const std::string& path);
		bool DeleteSound(const std::string& name);
		void PlaySound(const std::string& name);
	private:
		Engine* engine;
		SoLoud::Soloud soloud;
		SoLoud::Wav wav;

		std::unordered_map<std::string, SoLoud::Wav> nameToSoundMap;
	};
}
