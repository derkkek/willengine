#include "ResourceManager.h"
#include <string>
#include "../Engine.h"
#include "../SoundManager/SoundManager.h"
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <GraphicsManager/GraphicsManager.cpp>
#include <ScriptManager/ScriptManager.h>
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

	bool ResourceManager::LoadSound(const std::string& name, const std::string& relativePath)
	{
		const std::string resolvedPath = engine->resource->ResolvePath(relativePath);
		if (!resolvedPath.empty())
		{
			engine->sound->nameToSoundMap[name].load(resolvedPath.c_str());
			spdlog::info("sound: " + name + " has loaded");
			return true;
		}
		return false;
	}
	bool ResourceManager::DeleteSound(const std::string& name)
	{
		if (engine->sound->nameToSoundMap.contains(name))
		{
			engine->sound->nameToSoundMap.erase(name);
			return true;
		}
		spdlog::error("following sound isn't included in the sounds: " + name);
		return false;
	}

	bool ResourceManager::LoadTexture(const std::string& name, const std::string& relativePath)
	{
		std::string resolvedTexturePath = engine->resource->ResolvePath(relativePath);

		int width, height, channels;
		unsigned char* data = stbi_load(resolvedTexturePath.c_str(), &width, &height, &channels, 4);
		if (data == nullptr)
		{
			spdlog::error("Failed to load texture: {}", resolvedTexturePath);
			return false;
		}

		WGPUTexture tex = wgpuDeviceCreateTexture(engine->graphics->device, to_ptr(WGPUTextureDescriptor{
			.label = WGPUStringView(name.c_str(), WGPU_STRLEN),
			.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
			.dimension = WGPUTextureDimension_2D,
			.size = { (uint32_t)width, (uint32_t)height, 1 },
			.format = WGPUTextureFormat_RGBA8UnormSrgb,
			.mipLevelCount = 1,
			.sampleCount = 1
			}));

		wgpuQueueWriteTexture(
			engine->graphics->queue,
			to_ptr<WGPUTexelCopyTextureInfo>({ .texture = tex }),
			data,
			width * height * 4,
			to_ptr<WGPUTexelCopyBufferLayout>({ .bytesPerRow = (uint32_t)(width * 4), .rowsPerImage = (uint32_t)height }),
			to_ptr(WGPUExtent3D{ (uint32_t)width, (uint32_t)height, 1 })
		);

		stbi_image_free(data);

		// Store in map
		willengine::GraphicsManager::ImageData& img = engine->graphics->texturesMap[name];
		img.width = width;
		img.height = height;
		img.texture = tex;
		img.bindGroup = nullptr;  // Will be created on first use

		spdlog::info("Loaded texture '{}' ({}x{}) from {}", name, width, height, resolvedTexturePath);

		return true;
	}
	bool ResourceManager::DeleteTexture(const std::string& name)
	{
		if (engine->graphics->texturesMap.contains(name))
		{
			engine->graphics->texturesMap.erase(name);
			return true;
		}
		spdlog::error("following texture isn't included in the textures: " + name);
		return false;
	}

	bool ResourceManager::LoadScript(const std::string& name, const std::string& relativePath)
	{
		std::string resolvedPath = engine->resource->ResolvePath(relativePath);
		sol::load_result loadResult = engine->script->lua.load_file(resolvedPath);

		if (!loadResult.valid()) {
			sol::error err = loadResult;
			spdlog::error("Failed to load script '{}': {}", name, err.what());
			return false;
		}

		// Convert load_result to protected_function and store it
		sol::protected_function script = loadResult;
		engine->script->scripts[name] = script;

		spdlog::info("Loaded script '{}'", name);
		return true;
	}
	bool ResourceManager::DeleteScript(const std::string& name)
	{
		if (engine->script->scripts.contains(name))
		{
			engine->script->scripts.erase(name);
			return true;
		}
		spdlog::error("following script isn't included in the scripts: " + name);
		return false;
	}
}
