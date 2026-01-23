#pragma once
#include "../Engine.h"
#include <webgpu/webgpu.h>
#include <glfw3webgpu.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

namespace willengine
{
	class Engine;
	class ECS;
	struct Config;
	struct Sprite;


	class GraphicsManager
	{
		friend class InputManager;
		friend class ResourceManager;

	public:
		GraphicsManager(Engine* engine);
		~GraphicsManager();

		void Startup(Engine::Config config);
		void Shutdown();
		void Draw(const std::vector<Sprite>& sprites);
		void Draw();
		bool ShouldQuit();

		GLFWwindow* GetWindow() const { return window; }
		WGPUDevice GetDevice() const { return device; }
		WGPUQueue GetQueue() const { return queue; }
		WGPUTextureFormat GetSurfaceFormat() const;

		void DrawWithEditor(const std::function<void(WGPURenderPassEncoder)>& imguiRenderCallback);

	private:
		Engine* engine;
		
		GLFWwindow* window;

		WGPUInstance instance;
		WGPUSurface surface;
		WGPUAdapter adapter;
		WGPUQueue queue;
		WGPUDevice device;
		WGPUShaderModule shader_module;
		WGPURenderPipeline pipeline;
		WGPUBuffer vertex_buffer;
		WGPUBuffer uniform_buffer;
		WGPUSampler sampler;
		WGPUBuffer instance_buffer;
		size_t instance_buffer_capacity;
		
		// Background color
		double red = 0.1, green = 0.1, blue = 0.1;

		struct ImageData 
		{
			/*Preventing copying a texture between different structs so that releasing a texture second time won't cause crash.*/
			ImageData() = default;
			ImageData(const ImageData&) = delete;  // Can't copy
			ImageData& operator=(const ImageData&) = delete;  // Can't assign

			int width = 0;
			int height = 0;
			WGPUTexture texture = nullptr;
			WGPUBindGroup bindGroup = nullptr;
		};


		std::unordered_map<std::string, ImageData> texturesMap;
	};
}
