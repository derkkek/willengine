#include "UI.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"
#include <Events/CreateEntityEvent.h>
#include <Events/SaveEntityToConfigFileEvent.h>
#include "../EventHandler/EventHandler.h"

#include <spdlog/spdlog.h>
#include <cassert>
namespace willeditor
{
    EntityEditorState UI::g_entityEditor;

    UI::UI(App* app)
        :showEntityCreatorWindow(true), app(app)
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
        ShowMainToolbar();
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

            assert(app->eventHandler->EngineEmitCreateEntityEventCallback && "EngineEmitCreateEntityEventCallback not set!");
            app->eventHandler->EngineEmitCreateEntityEventCallback(data);

        }

        ImGui::SameLine();

        if (ImGui::Button("Save to Config"))
        {
            willengine::EntitySaveData saveData;
            saveData.entityID = g_entityEditor.entityID;

            if (g_entityEditor.hasTransform)
            {
                saveData.transform = willengine::vec2(g_entityEditor.transformX, g_entityEditor.transformY);
            }

            if (g_entityEditor.hasRigidbody)
            {
                saveData.rigidbody = willengine::Rigidbody(
                    willengine::vec2(g_entityEditor.rbPosX, g_entityEditor.rbPosY),
                    willengine::vec2(g_entityEditor.rbVelX, g_entityEditor.rbVelY)
                );
            }

            if (g_entityEditor.hasSprite)
            {
                saveData.sprite = willengine::Sprite(
                    g_entityEditor.spriteID,
                    g_entityEditor.spriteAlpha,
                    willengine::vec2(g_entityEditor.spriteWidth, g_entityEditor.spriteHeight)
                );
            }

            if (g_entityEditor.hasBoxCollider)
            {
                saveData.boxCollider = willengine::BoxCollider(
                    willengine::vec2(g_entityEditor.colliderWidth, g_entityEditor.colliderHeight),
                    false
                );
            }

            if (g_entityEditor.hasHealth)
            {
                saveData.health = willengine::Health(static_cast<double>(g_entityEditor.healthAmount));
            }

            if (g_entityEditor.hasScript)
            {
                willengine::Script scriptComponent;
                scriptComponent.name = g_entityEditor.scriptName;
                saveData.script = scriptComponent;
            }

            assert(app->eventHandler->EngineEmitSaveEntityCallback && "EngineEmitSaveEntityEventCallback not set!");
            app->eventHandler->EngineEmitSaveEntityCallback(saveData);
        }

        //ImGui::SameLine();

        //if (ImGui::Button("Reset"))
        //{
        //    g_entityEditor = EntityEditorState{};
        //}

        ImGui::End();
    }

    void UI::ShowMainToolbar()
    {
        // Create a toolbar window at the top
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 40));
        //ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags toolbar_flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 5));

        if (ImGui::Begin("##Toolbar", nullptr, toolbar_flags))
        {
            // Center the buttons
            float buttonWidth = 60.0f;
            float totalWidth = buttonWidth * 3 + ImGui::GetStyle().ItemSpacing.x * 2;
            float startX = (viewport->Size.x - totalWidth) / 2.0f;

            ImGui::SetCursorPosX(startX);

            // Play Button
            bool isPlaying = (playState == PlayState::Playing);
            bool isPaused = (playState == PlayState::Paused);
            bool isStopped = (playState == PlayState::Stopped);

            // Change button color when playing
            if (isPlaying)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
            }

            if (ImGui::Button(isPlaying ? "Playing" : "Play", ImVec2(buttonWidth, 25)))
            {
                if (isStopped || isPaused)
                {
                    playState = PlayState::Playing;
                    if (app->eventHandler->OnPlayClicked) app->eventHandler->OnPlayClicked();
                }
            }

            if (isPlaying)
            {
                ImGui::PopStyleColor(2);
            }

            ImGui::SameLine();

            // Pause Button
            if (isPaused)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.6f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.7f, 0.3f, 1.0f));
            }

            // Disable pause if not playing
            ImGui::BeginDisabled(isStopped);
            if (ImGui::Button(isPaused ? "Paused" : "Pause", ImVec2(buttonWidth, 25)))
            {
                if (isPlaying)
                {
                    playState = PlayState::Paused;
                    if (app->eventHandler->OnPauseClicked) app->eventHandler->OnPauseClicked();
                }
                else if (isPaused)
                {
                    playState = PlayState::Playing;
                    if (app->eventHandler->OnPlayClicked) app->eventHandler->OnPlayClicked();
                }
            }
            ImGui::EndDisabled();

            if (isPaused)
            {
                ImGui::PopStyleColor(2);
            }

            ImGui::SameLine();

            // Stop Button
            ImGui::BeginDisabled(isStopped);
            if (ImGui::Button("Stop", ImVec2(buttonWidth, 25)))
            {
                playState = PlayState::Stopped;
                if (app->eventHandler->OnStopClicked) app->eventHandler->OnStopClicked();
            }
            ImGui::EndDisabled();
        }
        ImGui::End();

        ImGui::PopStyleVar(3);
    }
}