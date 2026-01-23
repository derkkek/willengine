#include "Engine.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"
#include "GraphicsManager/GraphicsManager.h"
#include "PhysicsManager/PhysicsManager.h"
#include <iostream>
int main(int argc, const char* argv[])
{
    willengine::Engine engine{ willengine::Engine::Config{} };

	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();

    // Initialize GLFW backend
    ImGui_ImplGlfw_InitForOther(engine.graphics->GetWindow(), true);

    // Initialize WebGPU backend
    ImGui_ImplWGPU_InitInfo init_info{};
    init_info.Device = engine.graphics->GetDevice();
    init_info.RenderTargetFormat = engine.graphics->GetSurfaceFormat();
    init_info.DepthStencilFormat = WGPUTextureFormat_Undefined;  // No depth buffer
    init_info.NumFramesInFlight = 3;
    ImGui_ImplWGPU_Init(&init_info);


    engine.RunEditorLoop(
        [&]() {
            ImGui_ImplWGPU_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            engine.physics->Update();
            ImGui::ShowDemoWindow();

            ImGui::Render();
        },
        [](WGPURenderPassEncoder render_pass) {
            ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass);
        }
    );

    ImGui_ImplWGPU_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    engine.Shutdown();
    return 0;
}