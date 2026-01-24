#include "Engine.h"
#include "UserInterface/UI.h"
#include "GraphicsManager/GraphicsManager.h"
#include "PhysicsManager/PhysicsManager.h"
#include "States.h"
#include <iostream>
#include <Events/CreateEntityEvent.h>



int main(int argc, const char* argv[])
{
    willengine::Engine engine{ willengine::Engine::Config{} };

    willeditor::UI ui{};

    ui.Startup(engine.graphics->GetWindow(), engine.graphics->GetDevice(), engine.graphics->GetSurfaceFormat());

    ui.EngineEmitCreateEntityEventCallback = [&engine](const willengine::EntityCreationData& data) {
        engine.event->EmitEvent<willengine::CreateEntityEvent>(data);
        };

    engine.RunEditorLoop(
        [&]() {
            engine.physics->Update();
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