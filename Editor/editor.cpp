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
#include "App.h"
#include "EventHandler/EventHandler.h"


int main(int argc, const char* argv[])
{
    willengine::Engine engine{ willengine::Engine::Config{} };

    willeditor::App app;
    app.Startup(engine.graphics->GetWindow(), engine.graphics->GetDevice(), engine.graphics->GetSurfaceFormat());

    app.eventHandler->BindEventCallbacks({
    .onCreateEntity = [&engine](const willengine::EntityCreationData& data) { engine.event->EmitEvent<willengine::CreateEntityEvent>(data); },
    .onSaveEntity = [&engine](const willengine::EntitySaveData& saveData) { engine.event->EmitEvent<willengine::SaveEntityToConfigFileEvent>(saveData); },
    .onPlay = [&engine]() { engine.script->StartAllEntityScripts(); },
    .onPause = [&engine]() { spdlog::info("Pause clicked"); },
    .onStop = [&engine]() { spdlog::info("Stop clicked"); }
    //    // Optionally reset scene state here
        });

    engine.RunEditorLoop(
        [&]() 
        {
            if (app.ui->GetPlayState() == willeditor::PlayState::Playing)
            {
                engine.script->UpdateAllEntityScripts();
                engine.physics->Update();
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