#include "UI.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"
#include <Events/CreateEntityEvent.h>
#include "Types.h"
#include <spdlog/spdlog.h>
#include <cassert>
namespace willeditor
{
    EntityEditorState UI::g_entityEditor;

    UI::UI()
        :showEntityCreatorWindow(true)
    {
    }
    UI::~UI()
    {
    }
    void UI::Startup(GLFWwindow* window, WGPUDevice device, WGPUTextureFormat render_target_format)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();

        // Initialize GLFW backend
        ImGui_ImplGlfw_InitForOther(window, true);

        // Initialize WebGPU backend
        ImGui_ImplWGPU_InitInfo init_info{};
        init_info.Device = device;
        init_info.RenderTargetFormat = render_target_format;
        init_info.DepthStencilFormat = WGPUTextureFormat_Undefined;  // No depth buffer
        init_info.NumFramesInFlight = 3;
        ImGui_ImplWGPU_Init(&init_info);
	}

    void UI::Update()
    {
        Render();
    }
    void UI::Render()
    {
        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ShowEntityCreatorWindow(&showEntityCreatorWindow);

        ImGui::Render();
    }
    void UI::Shutdown()
    {
        ImGui_ImplWGPU_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    void UI::ShowEntityCreatorWindow(bool* open)
    {
        if (!ImGui::Begin("Entity Creator", open))
        {
            ImGui::End();
            return;
        }

        // Entity ID input
        ImGui::SeparatorText("Entity Identity");
        ImGui::InputText("Entity ID", g_entityEditor.entityID, IM_ARRAYSIZE(g_entityEditor.entityID));

        // Component selection
        ImGui::SeparatorText("Components");

        // Transform Component
        if (ImGui::Checkbox("Transform", &g_entityEditor.hasTransform))
        {
            // Auto-sync rigidbody position if both are enabled
        }
        if (g_entityEditor.hasTransform)
        {
            ImGui::Indent();
            ImGui::DragFloat2("Position##transform", &g_entityEditor.transformX, 0.1f);
            ImGui::Unindent();
        }

        // Rigidbody Component
        ImGui::Checkbox("Rigidbody", &g_entityEditor.hasRigidbody);
        if (g_entityEditor.hasRigidbody)
        {
            ImGui::Indent();
            ImGui::DragFloat2("Position##rb", &g_entityEditor.rbPosX, 0.1f);
            ImGui::DragFloat2("Velocity##rb", &g_entityEditor.rbVelX, 0.01f);
            ImGui::Unindent();
        }

        // Sprite Component
        ImGui::Checkbox("Sprite", &g_entityEditor.hasSprite);
        if (g_entityEditor.hasSprite)
        {
            ImGui::Indent();
            ImGui::InputText("Sprite ID", g_entityEditor.spriteID, IM_ARRAYSIZE(g_entityEditor.spriteID));
            ImGui::SliderFloat("Alpha", &g_entityEditor.spriteAlpha, 0.0f, 1.0f);
            ImGui::DragFloat("Width##sprite", &g_entityEditor.spriteWidth, 1.0f, 1.0f, 1000.0f);
            ImGui::DragFloat("Height##sprite", &g_entityEditor.spriteHeight, 1.0f, 1.0f, 1000.0f);
            ImGui::Unindent();
        }

        // BoxCollider Component
        ImGui::Checkbox("Box Collider", &g_entityEditor.hasBoxCollider);
        if (g_entityEditor.hasBoxCollider)
        {
            ImGui::Indent();
            ImGui::DragFloat("Width##collider", &g_entityEditor.colliderWidth, 1.0f, 1.0f, 1000.0f);
            ImGui::DragFloat("Height##collider", &g_entityEditor.colliderHeight, 1.0f, 1.0f, 1000.0f);
            ImGui::Unindent();
        }

        // Health Component
        ImGui::Checkbox("Health", &g_entityEditor.hasHealth);
        if (g_entityEditor.hasHealth)
        {
            ImGui::Indent();
            ImGui::DragFloat("Amount", &g_entityEditor.healthAmount, 1.0f, 0.0f, 1000.0f);
            ImGui::Unindent();
        }

        // Script Component
        ImGui::Checkbox("Script", &g_entityEditor.hasScript);
        if (g_entityEditor.hasScript)
        {
            ImGui::Indent();
            ImGui::InputText("Script File", g_entityEditor.scriptName, IM_ARRAYSIZE(g_entityEditor.scriptName));
            ImGui::Unindent();
        }

        ImGui::Separator();

        //// Action buttons
        if (ImGui::Button("Create Entity"))
        {
            // Add to scene (in-memory)
            willengine::EntityCreationData data;
            data.entityID = g_entityEditor.entityID;

            if (g_entityEditor.hasTransform) 
            {
                data.transform = willengine::vec2(g_entityEditor.transformX, g_entityEditor.transformY);
            }

            if (g_entityEditor.hasRigidbody) 
            {
                data.rigidbody = willengine::Rigidbody(
                    willengine::vec2(g_entityEditor.rbPosX, g_entityEditor.rbPosY),
                    willengine::vec2(g_entityEditor.rbVelX, g_entityEditor.rbVelY)
                );
            }

            if (g_entityEditor.hasSprite) 
            {
                data.sprite = willengine::Sprite(
                    g_entityEditor.spriteID,
                    g_entityEditor.spriteAlpha,
                    willengine::vec2(g_entityEditor.spriteWidth, g_entityEditor.spriteHeight)
                );
            }

            if (g_entityEditor.hasBoxCollider)
            {
                data.boxCollider = willengine::BoxCollider(
                    willengine::vec2(g_entityEditor.colliderWidth, g_entityEditor.colliderHeight),
                    false
                );
            }

            if (g_entityEditor.hasHealth) 
            {
                data.health = willengine::Health(static_cast<double>(g_entityEditor.healthAmount));
            }

            if (g_entityEditor.hasScript) 
            {
                willengine::Script scriptComponent;
                scriptComponent.name = g_entityEditor.scriptName;
                data.script = scriptComponent;
            }

            assert(EngineEmitCreateEntityEventCallback && "EngineEmitCreateEntityEventCallback not set!");
            EngineEmitCreateEntityEventCallback(data);

        }

        //ImGui::SameLine();

        //if (ImGui::Button("Save to Lua"))
        //{
        //    SaveSceneConfigToLua();
        //}

        //ImGui::SameLine();

        //if (ImGui::Button("Reset"))
        //{
        //    g_entityEditor = EntityEditorState{};
        //}

        ImGui::End();
    }
}