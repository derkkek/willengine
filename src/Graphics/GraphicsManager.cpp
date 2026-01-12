#include "GraphicsManager.h"
#include "../Engine.h"
#include "../ResourceManager/ResourceManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <algorithm>
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

	GraphicsManager::GraphicsManager(Engine* engine) : engine(engine), 
		instance_buffer(nullptr), instance_buffer_capacity(0)
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

		vertex_buffer = wgpuDeviceCreateBuffer(device, to_ptr(WGPUBufferDescriptor{
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

		uniform_buffer = wgpuDeviceCreateBuffer(device, to_ptr(WGPUBufferDescriptor{
	.label = WGPUStringView("Uniform Buffer", WGPU_STRLEN),
	.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
	.size = sizeof(Uniforms)
			}));

		sampler = wgpuDeviceCreateSampler(device, to_ptr(WGPUSamplerDescriptor{
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

		pipeline = wgpuDeviceCreateRenderPipeline(device, to_ptr(WGPURenderPipelineDescriptor{

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

		// Compute and upload the projection matrix
		Uniforms uniforms;
		uniforms.projection = mat4{1};
		uniforms.projection[0][0] = uniforms.projection[1][1] = 1./100.;
		
		glfwGetFramebufferSize(window, &width, &height);
		if( width < height ) {
			uniforms.projection[1][1] *= float(width);
			uniforms.projection[1][1] /= float(height);
		} else {
			uniforms.projection[0][0] *= float(height);
			uniforms.projection[0][0] /= float(width);
		}
		
		wgpuQueueWriteBuffer(queue, uniform_buffer, 0, &uniforms, sizeof(Uniforms));
	}

	void GraphicsManager::Shutdown()
	{
		// Release textures
		for (auto& pair : texturesMap) {
			if (pair.second.texture) wgpuTextureRelease(pair.second.texture);
			if (pair.second.bindGroup) wgpuBindGroupRelease(pair.second.bindGroup);
		}
		texturesMap.clear();
		
		if (instance_buffer) wgpuBufferRelease(instance_buffer);
		wgpuBufferRelease(vertex_buffer);
		wgpuBufferRelease(uniform_buffer);
		wgpuSamplerRelease(sampler);
		wgpuRenderPipelineRelease(pipeline);
		wgpuShaderModuleRelease(shader_module);
		wgpuQueueRelease(queue);
		wgpuDeviceRelease(device);
		wgpuAdapterRelease(adapter);
		wgpuSurfaceRelease(surface);
		wgpuInstanceRelease(instance);
		glfwTerminate();
	}
	void GraphicsManager::Draw(const std::vector<Sprite>& sprites_input)
	{
		// If no sprites, just clear the screen
		if (sprites_input.empty()) {
			WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, nullptr);
			WGPUSurfaceTexture surface_texture{};
			wgpuSurfaceGetCurrentTexture(surface, &surface_texture);
			WGPUTextureView current_texture_view = wgpuTextureCreateView(surface_texture.texture, nullptr);
			WGPURenderPassEncoder render_pass = wgpuCommandEncoderBeginRenderPass(encoder, to_ptr<WGPURenderPassDescriptor>({
				.colorAttachmentCount = 1,
				.colorAttachments = to_ptr<WGPURenderPassColorAttachment>({{
					.view = current_texture_view,
					.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
					.loadOp = WGPULoadOp_Clear,
					.storeOp = WGPUStoreOp_Store,
					.clearValue = WGPUColor{ red, green, blue, 1.0 }
					}})
				}));
			wgpuRenderPassEncoderEnd(render_pass);
			WGPUCommandBuffer command_buffer = wgpuCommandEncoderFinish(encoder, nullptr);
			wgpuQueueSubmit(queue, 1, &command_buffer);
			wgpuSurfacePresent(surface);
			
			wgpuTextureViewRelease(current_texture_view);
			wgpuTextureRelease(surface_texture.texture);
			wgpuRenderPassEncoderRelease(render_pass);
			wgpuCommandBufferRelease(command_buffer);
			wgpuCommandEncoderRelease(encoder);
			return;
		}

		// Sort sprites back-to-front (higher z first)
		auto sprites = sprites_input;
		std::sort(sprites.begin(), sprites.end(), 
			[](const Sprite& lhs, const Sprite& rhs) { return lhs.position.z > rhs.position.z; });

		// Allocate/reallocate instance buffer if needed
		if (instance_buffer_capacity < sprites.size()) {
			if (instance_buffer) wgpuBufferRelease(instance_buffer);
			instance_buffer_capacity = sprites.size();
			instance_buffer = wgpuDeviceCreateBuffer(device, to_ptr(WGPUBufferDescriptor{
				.label = WGPUStringView("Instance Buffer", WGPU_STRLEN),
				.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
				.size = sizeof(InstanceData) * instance_buffer_capacity
				}));
		}

		// Create command encoder
		WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, nullptr);
		
		// Get surface texture
		WGPUSurfaceTexture surface_texture{};
		wgpuSurfaceGetCurrentTexture(surface, &surface_texture);
		WGPUTextureView current_texture_view = wgpuTextureCreateView(surface_texture.texture, nullptr);
		
		// Begin render pass
		WGPURenderPassEncoder render_pass = wgpuCommandEncoderBeginRenderPass(encoder, to_ptr<WGPURenderPassDescriptor>({
			.colorAttachmentCount = 1,
			.colorAttachments = to_ptr<WGPURenderPassColorAttachment>({{
				.view = current_texture_view,
				.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
				.loadOp = WGPULoadOp_Clear,
				.storeOp = WGPUStoreOp_Store,
				.clearValue = WGPUColor{ red, green, blue, 1.0 }
				}})
			}));
		
		// Set pipeline and vertex buffers
		wgpuRenderPassEncoderSetPipeline(render_pass, pipeline);
		wgpuRenderPassEncoderSetVertexBuffer(render_pass, 0, vertex_buffer, 0, 4 * 4 * sizeof(float));
		wgpuRenderPassEncoderSetVertexBuffer(render_pass, 1, instance_buffer, 0, sizeof(InstanceData) * sprites.size());

		// Draw each sprite
		std::string current_image = "";
		for (size_t i = 0; i < sprites.size(); ++i) {
			const Sprite& sprite = sprites[i];
			
			// Check if texture exists
			auto it = texturesMap.find(sprite.image);
			if (it == texturesMap.end()) {
				spdlog::warn("Texture '{}' not found, skipping sprite", sprite.image);
				continue;
			}
			
			const ImageData& image_data = it->second;
			
			// Compute instance data
			InstanceData instance_data;
			instance_data.translation = sprite.position;
			
			// Scale to maintain aspect ratio
			vec2 aspect_scale;
			if (image_data.width < image_data.height) {
				aspect_scale = vec2(float(image_data.width) / image_data.height, 1.0f);
			} else {
				aspect_scale = vec2(1.0f, float(image_data.height) / image_data.width);
			}
			instance_data.scale = aspect_scale * sprite.scale;
			
			// Upload instance data to GPU
			wgpuQueueWriteBuffer(queue, instance_buffer, i * sizeof(InstanceData), &instance_data, sizeof(InstanceData));
			
			// Set bind group if image changed
			if (sprite.image != current_image) {
				current_image = sprite.image;
				
				// Create bind group if it doesn't exist
				if (!image_data.bindGroup) {
					auto layout = wgpuRenderPipelineGetBindGroupLayout(pipeline, 0);
					WGPUBindGroup bind_group = wgpuDeviceCreateBindGroup(device, to_ptr(WGPUBindGroupDescriptor{
						.layout = layout,
						.entryCount = 3,
						.entries = to_ptr<WGPUBindGroupEntry>({
							{
								.binding = 0,
								.buffer = uniform_buffer,
								.size = sizeof(Uniforms)
							},
							{
								.binding = 1,
								.sampler = sampler,
							},
							{
								.binding = 2,
								.textureView = wgpuTextureCreateView(image_data.texture, nullptr)
							}
							})
						}));
					wgpuBindGroupLayoutRelease(layout);
					
					// Store it (need to cast away const since we're caching)
					const_cast<ImageData&>(image_data).bindGroup = bind_group;
				}
				
				wgpuRenderPassEncoderSetBindGroup(render_pass, 0, image_data.bindGroup, 0, nullptr);
			}
			
			// Draw the sprite instance
			wgpuRenderPassEncoderDraw(render_pass, 4, 1, 0, (uint32_t)i);
		}
		
		// End render pass
		wgpuRenderPassEncoderEnd(render_pass);
		
		// Submit commands
		WGPUCommandBuffer command_buffer = wgpuCommandEncoderFinish(encoder, nullptr);
		wgpuQueueSubmit(queue, 1, &command_buffer);
		
		// Present
		wgpuSurfacePresent(surface);
		
		// Cleanup
		wgpuTextureViewRelease(current_texture_view);
		wgpuTextureRelease(surface_texture.texture);
		wgpuRenderPassEncoderRelease(render_pass);
		wgpuCommandBufferRelease(command_buffer);
		wgpuCommandEncoderRelease(encoder);
	}

	void GraphicsManager::Draw()
	{
		// No-parameter version - just clears the screen
		Draw(std::vector<Sprite>());
	}
	bool GraphicsManager::ShouldQuit()
	{
		return glfwWindowShouldClose(window);
	}
	bool GraphicsManager::LoadTexture(const std::string& name, const std::string& path)
	{
		std::string resolvedTexturePath = engine->resource->ResolvePath(path);

		int width, height, channels;
		unsigned char* data = stbi_load(resolvedTexturePath.c_str(), &width, &height, &channels, 4);
		if (data == nullptr)
		{
			spdlog::error("Failed to load texture: {}", resolvedTexturePath);
			return false;
		}

		WGPUTexture tex = wgpuDeviceCreateTexture(device, to_ptr(WGPUTextureDescriptor{
			.label = WGPUStringView(name.c_str(), WGPU_STRLEN),
			.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
			.dimension = WGPUTextureDimension_2D,
			.size = { (uint32_t)width, (uint32_t)height, 1 },
			.format = WGPUTextureFormat_RGBA8UnormSrgb,
			.mipLevelCount = 1,
			.sampleCount = 1
			}));

		wgpuQueueWriteTexture(
			queue,
			to_ptr<WGPUTexelCopyTextureInfo>({ .texture = tex }),
			data,
			width * height * 4,
			to_ptr<WGPUTexelCopyBufferLayout>({ .bytesPerRow = (uint32_t)(width * 4), .rowsPerImage = (uint32_t)height }),
			to_ptr(WGPUExtent3D{ (uint32_t)width, (uint32_t)height, 1 })
		);

		stbi_image_free(data);

		// Store in map
		ImageData& img = texturesMap[name];
		img.width = width;
		img.height = height;
		img.texture = tex;
		img.bindGroup = nullptr;  // Will be created on first use

		spdlog::info("Loaded texture '{}' ({}x{}) from {}", name, width, height, resolvedTexturePath);

		return true;
	}
	void GraphicsManager::AddSprite(const std::string& name, vec3 position, vec2 scale, const std::string& path)
	{
		if (LoadTexture(name, path))
		{
			sprites.push_back(Sprite{ name, position, scale });
		}
	}

	}
