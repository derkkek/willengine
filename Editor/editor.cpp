#include "Engine.h"
#include "UserInterface/UI.h"
#include "GraphicsManager/GraphicsManager.h"
#include "PhysicsManager/PhysicsManager.h"
#include "ScriptManager/ScriptManager.h"
#include "States.h"
#include <iostream>
#include <Events/CreateEntityEvent.h>
#include <Events/SaveSceneEvent.h>
#include <spdlog/spdlog.h>
#include "App.h"
#include "EventHandler/EventHandler.h"
#include <SceneManager/SceneManager.h>

int main(int argc, const char* argv[])
{
    willengine::Engine engine{ willengine::Engine::Config{} };

    willeditor::App app;
    app.Startup(engine.graphics->GetWindow(), engine.graphics->GetDevice(), engine.graphics->GetSurfaceFormat());

    app.eventHandler->BindEventCallbacks({
    .onCreateEntity = [&engine](const willengine::EntityCreationData& data) {
        engine.event->EmitEvent<willengine::CreateEntityEvent>(data);
    },
    .onSaveScene = [&engine]() {
        engine.event->EmitEvent<willengine::SaveSceneEvent>();
    },
    .onPlay = [&engine]() {
        spdlog::info("Play clicked - starting scripts");
        engine.script->StartAllEntityScripts();
    },
    .onPause = []() {
        spdlog::info("Pause clicked");
    },
    .onStop = []() {
        spdlog::info("Stop clicked");
    },
        // Clean one-liner calls to SceneManager
        .onModifyTransform = [&engine](const std::string& name, float x, float y) {
            engine.scene->ModifyTransform(name, x, y);
        },
        .onModifyRigidbody = [&engine](const std::string& name, float posX, float posY, float velX, float velY) {
            engine.scene->ModifyRigidbody(name, posX, posY, velX, velY);
        },
        .onModifySprite = [&engine](const std::string& name, const std::string& image, float alpha, float scaleX, float scaleY) {
            engine.scene->ModifySprite(name, image, alpha, scaleX, scaleY);
        },
        .onModifyBoxCollider = [&engine](const std::string& name, float width, float height) {
            engine.scene->ModifyBoxCollider(name, width, height);
        },
        .onModifyHealth = [&engine](const std::string& name, float amount) {
            engine.scene->ModifyHealth(name, amount);
        },
        .onModifyScript = [&engine](const std::string& name, const std::string& scriptName) {
            engine.scene->ModifyScript(name, scriptName);
        }
        });

    std::vector<willeditor::UI::EntityDisplayInfo> entityList;
    entityList.reserve(256);

    for (auto& [name, id] : engine.scene->GetNamedEntities()) {
        willeditor::UI::EntityDisplayInfo info;
        info.name = name;
        info.id = id;
        info.hasTransform = engine.ecs.Has<willengine::Transform>(id);
        info.hasRigidbody = engine.ecs.Has<willengine::Rigidbody>(id);
        info.hasSprite = engine.ecs.Has<willengine::Sprite>(id);
        info.hasBoxCollider = engine.ecs.Has<willengine::BoxCollider>(id);
        info.hasHealth = engine.ecs.Has<willengine::Health>(id);
        info.hasScript = engine.ecs.Has<willengine::Script>(id);
        entityList.emplace_back(info);
    }
    app.ui->SetEntityList(entityList);

    engine.RunEditorLoop(
        [&]()
        {
            // Update physics/scripts only when playing
            if (app.ui->GetPlayState() == willeditor::PlayState::Playing)
            {
                engine.script->UpdateAllEntityScripts();
                engine.physics->Update();
            }

            // Only rebuild entity list when something changed
            if (engine.scene->IsDirty())
            {
                std::vector<willeditor::UI::EntityDisplayInfo> entityList;

                for (auto& [name, id] : engine.scene->GetNamedEntities()) {
                    willeditor::UI::EntityDisplayInfo info;
                    info.name = name;
                    info.id = id;

                    // Transform
                    if (engine.ecs.Has<willengine::Transform>(id)) {
                        info.hasTransform = true;
                        willengine::Transform& t = engine.ecs.Get<willengine::Transform>(id);
                        info.transformX = t.x;
                        info.transformY = t.y;
                    }

                    // Rigidbody
                    if (engine.ecs.Has<willengine::Rigidbody>(id)) {
                        info.hasRigidbody = true;
                        willengine::Rigidbody& rb = engine.ecs.Get<willengine::Rigidbody>(id);
                        info.rbPosX = rb.position.x;
                        info.rbPosY = rb.position.y;
                        info.rbVelX = rb.velocity.x;
                        info.rbVelY = rb.velocity.y;
                    }

                    // Sprite
                    if (engine.ecs.Has<willengine::Sprite>(id)) {
                        info.hasSprite = true;
                        willengine::Sprite& s = engine.ecs.Get<willengine::Sprite>(id);
                        info.spriteImage = s.image;
                        info.spriteAlpha = s.alpha;
                        info.spriteScaleX = s.scale.x;
                        info.spriteScaleY = s.scale.y;
                    }

                    // BoxCollider
                    if (engine.ecs.Has<willengine::BoxCollider>(id)) {
                        info.hasBoxCollider = true;
                        willengine::BoxCollider& bc = engine.ecs.Get<willengine::BoxCollider>(id);
                        info.colliderWidth = bc.dimensionSizes.x;
                        info.colliderHeight = bc.dimensionSizes.y;
                    }

                    // Health
                    if (engine.ecs.Has<willengine::Health>(id)) {
                        info.hasHealth = true;
                        willengine::Health& h = engine.ecs.Get<willengine::Health>(id);
                        info.healthAmount = static_cast<float>(h.percent);
                    }

                    // Script
                    if (engine.ecs.Has<willengine::Script>(id)) {
                        info.hasScript = true;
                        willengine::Script& script = engine.ecs.Get<willengine::Script>(id);
                        info.scriptName = script.name;
                    }

                    entityList.push_back(info);
                }

                app.ui->SetEntityList(entityList);
                engine.scene->ClearDirty();  // Reset the flag after rebuilding
            }

            app.Update();
        },
        [](WGPURenderPassEncoder render_pass) {
            ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass);
        }
    );
    
    app.ui->Shutdown();
    engine.Shutdown();
    return 0;
}