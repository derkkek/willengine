#include "UI.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"
#include <Events/CreateEntityEvent.h>
#include <Events/SaveSceneEvent.h>
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
        ShowEntitiesList(&showHierarchyWindow);
        ShowInspectorWindow(&showInspectorWindow);
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

        // Check if we're in play mode
        bool isPlayMode = (playState != PlayState::Stopped);

        // Show warning if in play mode
        if (isPlayMode)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
            ImGui::Text("Disabled during play mode");
            ImGui::PopStyleColor();
            ImGui::Separator();
        }

        // Disable all editing widgets when playing
        ImGui::BeginDisabled(isPlayMode);

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

        if (ImGui::Button("Save Scene"))
        {
            assert(app->eventHandler->EngineEmitSaveSceneCallback && "EngineEmitSaveSceneCallback not set!");
            app->eventHandler->EngineEmitSaveSceneCallback();
        }

        //ImGui::SameLine();

        //if (ImGui::Button("Reset"))
        //{
        //    g_entityEditor = EntityEditorState{};
        //}

        ImGui::EndDisabled();

        ImGui::End();
    }

    void UI::ShowEntitiesList(bool* open)
    {
        ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);

        if (!ImGui::Begin("Hierarchy", open))
        {
            ImGui::End();
            return;
        }

        // Header with entity count
        ImGui::Text("Scene Entities (%zu)", entities.size());
        ImGui::Separator();

        // Search filter (optional but nice to have)
        static char searchFilter[128] = "";
        ImGui::InputTextWithHint("##search", "Search...", searchFilter, IM_ARRAYSIZE(searchFilter));
        ImGui::Separator();

        // Entity list
        for (const auto& entity : entities)
        {
            // Apply search filter
            if (strlen(searchFilter) > 0)
            {
                if (entity.name.find(searchFilter) == std::string::npos)
                    continue;
            }

            // Create a selectable item for each entity
            bool isSelected = (selectedEntityName == entity.name);

            // Build a label showing entity name and icon hints
            std::string label = entity.name;

            // Use tree node for expandable view, or Selectable for flat list
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (isSelected)
                flags |= ImGuiTreeNodeFlags_Selected;

            // Add small icons/hints for components
            ImGui::PushID(entity.name.c_str());

            if (ImGui::TreeNodeEx(label.c_str(), flags))
            {
                // Handle selection
                if (ImGui::IsItemClicked())
                {
                    selectedEntityName = entity.name;
                }
            }

            // Right-click context menu
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Delete Entity"))
                {
                    // TODO: Emit delete entity event
                    spdlog::info("Delete requested for: {}", entity.name);
                }
                if (ImGui::MenuItem("Duplicate"))
                {
                    // TODO: Emit duplicate entity event
                }
                ImGui::EndPopup();
            }

            // Tooltip showing components
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("ID: %ld", entity.id);
                ImGui::Text("Components:");
                if (entity.hasTransform)   ImGui::BulletText("Transform");
                if (entity.hasRigidbody)   ImGui::BulletText("Rigidbody");
                if (entity.hasSprite)      ImGui::BulletText("Sprite");
                if (entity.hasBoxCollider) ImGui::BulletText("BoxCollider");
                if (entity.hasHealth)      ImGui::BulletText("Health");
                if (entity.hasScript)      ImGui::BulletText("Script");
                ImGui::EndTooltip();
            }

            ImGui::PopID();
        }

        ImGui::End();
    }
    void UI::ShowInspectorWindow(bool* open)
    {
        ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);

        if (!ImGui::Begin("Inspector", open))
        {
            ImGui::End();
            return;
        }

        if (selectedEntityName.empty())
        {
            ImGui::TextDisabled("No entity selected");
            ImGui::TextDisabled("Select an entity from the Hierarchy");
            ImGui::End();
            return;
        }

        // Find the selected entity (get non-const pointer so we can edit)
        EntityDisplayInfo* selected = nullptr;
        for (auto& e : entities)
        {
            if (e.name == selectedEntityName)
            {
                selected = &e;
                break;
            }
        }

        if (!selected)
        {
            selectedEntityName = "";  // Clear invalid selection
            ImGui::TextDisabled("Entity not found");
            ImGui::End();
            return;
        }

        // Entity header
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
        ImGui::Text("%s", selected->name.c_str());
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextDisabled("(ID: %ld)", selected->id);
        ImGui::Separator();
        ImGui::Spacing();

        // Check if we're in play mode
        bool isPlayMode = (playState != PlayState::Stopped);

        // Show warning if in play mode
        if (isPlayMode)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
            ImGui::Text("Read-only during play mode");
            ImGui::PopStyleColor();
            ImGui::Separator();
            ImGui::Spacing();
        }

        // Disable all editing widgets when playing
        ImGui::BeginDisabled(isPlayMode);

        // ==================== TRANSFORM ====================
        if (selected->hasTransform)
        {
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();

                float pos[2] = { selected->transformX, selected->transformY };
                if (ImGui::DragFloat2("Position", pos, 0.1f))
                {
                    selected->transformX = pos[0];
                    selected->transformY = pos[1];

                    // Notify engine of change
                    assert(app->eventHandler->OnModifyTransform);
                    app->eventHandler->OnModifyTransform(selected->name, pos[0], pos[1]);
                }

                ImGui::Unindent();
            }
        }

        // ==================== RIGIDBODY ====================
        if (selected->hasRigidbody)
        {
            if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();

                float rbPos[2] = { selected->rbPosX, selected->rbPosY };
                if (ImGui::DragFloat2("Position##rb", rbPos, 0.1f))
                {
                    selected->rbPosX = rbPos[0];
                    selected->rbPosY = rbPos[1];

                    assert(app->eventHandler->OnModifyRigidbody);
                    
                    app->eventHandler->OnModifyRigidbody(selected->name, rbPos[0], rbPos[1],
                            selected->rbVelX, selected->rbVelY);
                    
                }

                float rbVel[2] = { selected->rbVelX, selected->rbVelY };
                if (ImGui::DragFloat2("Velocity##rb", rbVel, 0.01f))
                {
                    selected->rbVelX = rbVel[0];
                    selected->rbVelY = rbVel[1];

                    app->eventHandler->OnModifyRigidbody(selected->name, selected->rbPosX, selected->rbPosY,
                            rbVel[0], rbVel[1]);
                }

                ImGui::Unindent();
            }
        }

        // ==================== SPRITE ====================
        if (selected->hasSprite)
        {
            if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();

                // Sprite image (read-only display, or use dropdown for available sprites)
                ImGui::Text("Image: %s", selected->spriteImage.c_str());

                // Alpha slider
                if (ImGui::SliderFloat("Alpha", &selected->spriteAlpha, 0.0f, 1.0f))
                {
                    assert(app->eventHandler->OnModifySprite);
                        app->eventHandler->OnModifySprite(selected->name, selected->spriteImage,
                            selected->spriteAlpha,
                            selected->spriteScaleX, selected->spriteScaleY);
                }

                // Scale
                float scale[2] = { selected->spriteScaleX, selected->spriteScaleY };
                if (ImGui::DragFloat2("Scale", scale, 0.5f, 1.0f, 1000.0f))
                {
                    selected->spriteScaleX = scale[0];
                    selected->spriteScaleY = scale[1];

                    assert(app->eventHandler->OnModifySprite);
                    app->eventHandler->OnModifySprite(selected->name, selected->spriteImage,
                            selected->spriteAlpha, scale[0], scale[1]);
                }

                ImGui::Unindent();
            }
        }

        // ==================== BOX COLLIDER ====================
        if (selected->hasBoxCollider)
        {
            if (ImGui::CollapsingHeader("Box Collider", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();

                float size[2] = { selected->colliderWidth, selected->colliderHeight };
                if (ImGui::DragFloat2("Size", size, 0.5f, 1.0f, 1000.0f))
                {
                    selected->colliderWidth = size[0];
                    selected->colliderHeight = size[1];

                    assert(app->eventHandler->OnModifyBoxCollider);
                    app->eventHandler->OnModifyBoxCollider(selected->name, size[0], size[1]);
                }

                ImGui::Unindent();
            }
        }

        // ==================== HEALTH ====================
        if (selected->hasHealth)
        {
            if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();

                if (ImGui::DragFloat("Amount", &selected->healthAmount, 1.0f, 0.0f, 10000.0f))
                {
                    assert(app->eventHandler->OnModifyHealth);
                    app->eventHandler->OnModifyHealth(selected->name, selected->healthAmount);
                }

                // Visual health bar
                ImGui::ProgressBar(selected->healthAmount / 100.0f, ImVec2(-1, 0), "");

                ImGui::Unindent();
            }
        }

        // ==================== SCRIPT ====================
        if (selected->hasScript)
        {
            if (ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();

                ImGui::Text("Script: %s", selected->scriptName.c_str());
                // Scripts are typically not edited at runtime, just displayed

                ImGui::Unindent();
            }
        }

        ImGui::EndDisabled();

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