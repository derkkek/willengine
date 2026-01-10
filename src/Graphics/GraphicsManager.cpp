#include "GraphicsManager.h"
#include "../Engine.h"
#include <iostream>
#include <spdlog/spdlog.h>
#include <webgpu/webgpu.h>
#include <glfw3webgpu.h>
namespace
{
	struct InstanceData {
		willengine::vec3 translation;
		willengine::vec2 scale;
		// rotation?
	};

	struct Uniforms {
		willengine::mat4 projection;
	};
}
namespace willengine
{

	template< typename T > constexpr const T* to_ptr(const T& val) { return &val; }
	template< typename T, std::size_t N > constexpr const T* to_ptr(const T(&& arr)[N]) { return arr; }

	GraphicsManager::GraphicsManager(Engine* engine) : engine(engine)
	{
	}

	GraphicsManager::~GraphicsManager()
	{
	}

	WGPUTextureFormat wgpuSurfaceGetPreferredFormat(WGPUSurface surface, WGPUAdapter adapter) {
		WGPUSurfaceCapabilities capabilities{};
		wgpuSurfaceGetCapabilities(surface, adapter, &capabilities);
		const WGPUTextureFormat result = capabilities.formats[0];
		wgpuSurfaceCapabilitiesFreeMembers(capabilities);
		return result;
	}

	void GraphicsManager::Startup(Engine::Config config)
	{
		glfwInit();
		// We don't want GLFW to set up a graphics API.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		// Create the window.
		window = glfwCreateWindow(config.window_width, config.window_height, config.window_name.c_str(), config.window_fullscreen ? glfwGetPrimaryMonitor() : 0, 0);
		if (!window)
		{
			spdlog::error("window initialization has been failed.");
			glfwTerminate();
		}
		glfwSetWindowAspectRatio(window, config.window_width, config.window_height);


		instance = wgpuCreateInstance(to_ptr(WGPUInstanceDescriptor{}));
		surface = glfwCreateWindowWGPUSurface(instance, window);

		adapter = nullptr;

		wgpuInstanceRequestAdapter(
			instance,
			to_ptr(WGPURequestAdapterOptions{ .featureLevel = WGPUFeatureLevel_Core, .compatibleSurface = surface }),
			WGPURequestAdapterCallbackInfo{
				.mode = WGPUCallbackMode_AllowSpontaneous,
				.callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* adapter_ptr, void*) {
					if (status != WGPURequestAdapterStatus_Success) {
						std::cerr << "Failed to get a WebGPU adapter: " << std::string_view(message.data, message.length) << std::endl;
						glfwTerminate();
					}

					*static_cast<WGPUAdapter*>(adapter_ptr) = adapter;
				},
				.userdata1 = &(adapter)
			}
		);
		while (!adapter) wgpuInstanceProcessEvents(instance);
		assert(adapter);

		wgpuAdapterRequestDevice(
			adapter,
			to_ptr(WGPUDeviceDescriptor{
				// Add an error callback for more debug info
				.uncapturedErrorCallbackInfo = {.callback = [](WGPUDevice const* device, WGPUErrorType type, WGPUStringView message, void*, void*) {
					std::cerr << "WebGPU uncaptured error type " << int(type) << " with message: " << std::string_view(message.data, message.length) << std::endl;
				}}
				}),
			WGPURequestDeviceCallbackInfo{
				.mode = WGPUCallbackMode_AllowSpontaneous,
				.callback = [](WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* device_ptr, void*) {
					if (status != WGPURequestDeviceStatus_Success) {
						std::cerr << "Failed to get a WebGPU device: " << std::string_view(message.data, message.length) << std::endl;
						glfwTerminate();
					}

					*static_cast<WGPUDevice*>(device_ptr) = device;
				},
				.userdata1 = &(device)
			}
		);
		while (!device) wgpuInstanceProcessEvents(instance);
		assert(device);

		queue = wgpuDeviceGetQueue(device);

		// A vertex buffer containing a textured square.
		const struct {
			// position
			float x, y;
			// texcoords
			float u, v;
		} 
		
		vertices[] = {
			// position       // texcoords
		  { -1.0f,  -1.0f,    0.0f,  1.0f },
		  {  1.0f,  -1.0f,    1.0f,  1.0f },
		  { -1.0f,   1.0f,    0.0f,  0.0f },
		  {  1.0f,   1.0f,    1.0f,  0.0f },
		};

		WGPUBuffer vertex_buffer = wgpuDeviceCreateBuffer(device, to_ptr(WGPUBufferDescriptor{
	.label = WGPUStringView("Vertex Buffer", WGPU_STRLEN),
	.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
	.size = sizeof(vertices)
			}));

		wgpuQueueWriteBuffer(queue, vertex_buffer, 0, vertices, sizeof(vertices));

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		wgpuSurfaceConfigure(surface, to_ptr(WGPUSurfaceConfiguration{
			.device = device,
			.format = wgpuSurfaceGetPreferredFormat(surface, adapter),
			.usage = WGPUTextureUsage_RenderAttachment,
			.width = (uint32_t)width,
			.height = (uint32_t)height,
			.presentMode = WGPUPresentMode_Fifo // Explicitly set this because of a Dawn bug
			}));

		WGPUBuffer uniform_buffer = wgpuDeviceCreateBuffer(device, to_ptr(WGPUBufferDescriptor{
	.label = WGPUStringView("Uniform Buffer", WGPU_STRLEN),
	.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
	.size = sizeof(Uniforms)
			}));

		WGPUSampler sampler = wgpuDeviceCreateSampler(device, to_ptr(WGPUSamplerDescriptor{
	.addressModeU = WGPUAddressMode_ClampToEdge,
	.addressModeV = WGPUAddressMode_ClampToEdge,
	.magFilter = WGPUFilterMode_Linear,
	.minFilter = WGPUFilterMode_Linear,
	.maxAnisotropy = 1
			}));

		const char* source = R"(
			struct Uniforms {
				projection: mat4x4f,
			};

			@group(0) @binding(0) var<uniform> uniforms: Uniforms;
			@group(0) @binding(1) var texSampler: sampler;
			@group(0) @binding(2) var texData: texture_2d<f32>;

			struct VertexInput {
				@location(0) position: vec2f,
				@location(1) texcoords: vec2f,
				@location(2) translation: vec3f,
				@location(3) scale: vec2f,
			};

			struct VertexOutput {
				@builtin(position) position: vec4f,
				@location(0) texcoords: vec2f,
			};

			@vertex
			fn vertex_shader_main( in: VertexInput ) -> VertexOutput {
				var out: VertexOutput;
				out.position = uniforms.projection * vec4f( vec3f( in.scale * in.position, 0.0 ) + in.translation, 1.0 );
				out.texcoords = in.texcoords;
				return out;
			}

			@fragment
			fn fragment_shader_main( in: VertexOutput ) -> @location(0) vec4f {
				let color = textureSample( texData, texSampler, in.texcoords ).rgba;
				return color;
			}
			)";

		WGPUShaderSourceWGSL source_desc = {};
		source_desc.chain.sType = WGPUSType_ShaderSourceWGSL;
		source_desc.code = WGPUStringView(source, std::string_view(source).length());
		// Point to the code descriptor from the shader descriptor.
		WGPUShaderModuleDescriptor shader_desc = {};
		shader_desc.nextInChain = &source_desc.chain;
		shader_module = wgpuDeviceCreateShaderModule(device, &shader_desc);

		WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(device, to_ptr(WGPURenderPipelineDescriptor{

			// Describe the vertex shader inputs
			.vertex = {
				.module = shader_module,
				.entryPoint = WGPUStringView{ "vertex_shader_main", std::string_view("vertex_shader_main").length() },
				// Vertex attributes.
				.bufferCount = 2,
				.buffers = to_ptr<WGPUVertexBufferLayout>({
				// We have one buffer with our per-vertex position and UV data. This data never changes.
				// Note how the type, byte offset, and stride (bytes between elements) exactly matches our `vertex_buffer`.
				{
					.stepMode = WGPUVertexStepMode_Vertex,
					.arrayStride = 4 * sizeof(float),
					.attributeCount = 2,
					.attributes = to_ptr<WGPUVertexAttribute>({
						// Position x,y are first.
						{
							.format = WGPUVertexFormat_Float32x2,
							.offset = 0,
							.shaderLocation = 0
						},
						// Texture coordinates u,v are second.
						{
							.format = WGPUVertexFormat_Float32x2,
							.offset = 2 * sizeof(float),
							.shaderLocation = 1
						}
						})
				},
					// We will use a second buffer with our per-sprite translation and scale. This data will be set in our draw function.
					{
						// This data is per-instance. All four vertices will get the same value. Each instance of drawing the vertices will get a different value.
						// The type, byte offset, and stride (bytes between elements) exactly match the array of `InstanceData` structs we will upload in our draw function.
						.stepMode = WGPUVertexStepMode_Instance,
						.arrayStride = sizeof(InstanceData),
						.attributeCount = 2,
						.attributes = to_ptr<WGPUVertexAttribute>({
						// Translation as a 3D vector.
						{
							.format = WGPUVertexFormat_Float32x3,
							.offset = offsetof(InstanceData, translation),
							.shaderLocation = 2
						},
							// Scale as a 2D vector for non-uniform scaling.
							{
								.format = WGPUVertexFormat_Float32x2,
								.offset = offsetof(InstanceData, scale),
								.shaderLocation = 3
							}
							})
					}
					})
				},

			// Interpret our 4 vertices as a triangle strip
			.primitive = WGPUPrimitiveState{
				.topology = WGPUPrimitiveTopology_TriangleStrip,
				},

				// No multi-sampling (1 sample per pixel, all bits on).
				.multisample = WGPUMultisampleState{
					.count = 1,
					.mask = ~0u
					},

			// Describe the fragment shader and its output
			.fragment = to_ptr(WGPUFragmentState{
				.module = shader_module,
				.entryPoint = WGPUStringView{ "fragment_shader_main", std::string_view("fragment_shader_main").length() },

				// Our fragment shader outputs a single color value per pixel.
				.targetCount = 1,
				.targets = to_ptr<WGPUColorTargetState>({
					{
						.format = wgpuSurfaceGetPreferredFormat(surface, adapter),
						// The images we want to draw may have transparency, so let's turn on alpha blending with over compositing (ɑ⋅foreground + (1-ɑ)⋅background).
						// This will blend with whatever has already been drawn.
						.blend = to_ptr(WGPUBlendState{
						// Over blending for color
						.color = {
							.operation = WGPUBlendOperation_Add,
							.srcFactor = WGPUBlendFactor_SrcAlpha,
							.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha
							},
							// Leave destination alpha alone
							.alpha = {
								.operation = WGPUBlendOperation_Add,
								.srcFactor = WGPUBlendFactor_Zero,
								.dstFactor = WGPUBlendFactor_One
								}
							}),
						.writeMask = WGPUColorWriteMask_All
					}})
				})
			}));
	}

	void GraphicsManager::Shutdown()
	{
		glfwTerminate();
		wgpuInstanceRelease(instance);
		wgpuSurfaceRelease(surface);
		wgpuAdapterRelease(adapter);
		wgpuDeviceRelease(device);
		wgpuQueueRelease(queue);
		wgpuShaderModuleRelease(shader_module);
	}
	void GraphicsManager::Draw()
	{

	}
	bool GraphicsManager::ShouldQuit()
	{
		return glfwWindowShouldClose(window);
	}
}
