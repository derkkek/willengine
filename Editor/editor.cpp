#include "Engine.h"
#include "UserInterface/UI.h"
#include "GraphicsManager/GraphicsManager.h"
#include "PhysicsManager/PhysicsManager.h"
#include "ScriptManager/ScriptManager.h"
#include "States.h"
#include <iostream>
#include <Events/CreateEntityEvent.h>
#include <Events/SaveEntityToConfigFileEvent.h>
#include <spdlog/spdlog.h>



int main(int argc, const char* argv[])
{
    willengine::Engine engine{ willengine::Engine::Config{} };

    willeditor::UI ui{};

    ui.Startup(engine.graphics->GetWindow(), engine.graphics->GetDevice(), engine.graphics->GetSurfaceFormat());

    ui.EngineEmitCreateEntityEventCallback = [&engine](const willengine::EntityCreationData& data) {
        engine.event->EmitEvent<willengine::CreateEntityEvent>(data);
        };
    ui.EngineEmitSaveEntityCallback = [&engine](const willengine::EntitySaveData& saveData) {
        engine.event->EmitEvent<willengine::SaveEntityToConfigFileEvent>(saveData);
        };

    ui.OnPlayClicked = [&engine]() {
        spdlog::info("Play clicked - starting scripts");
        engine.script->StartAllEntityScripts();
        };

    ui.OnPauseClicked = [&engine]() {
        spdlog::info("Pause clicked");
        };

    ui.OnStopClicked = [&engine]() {
        spdlog::info("Stop clicked");
        // Optionally reset scene state here
        };

    engine.RunEditorLoop(
        [&]() 
        {
            if (ui.GetPlayState() == willeditor::PlayState::Playing)
            {
                engine.script->UpdateAllEntityScripts();
                engine.physics->Update();
            }
            ui.Update();
        },
        [](WGPURenderPassEncoder render_pass) {
            ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass);
        }
    );

    
    ui.Shutdown();
    engine.Shutdown();
    return 0;
}