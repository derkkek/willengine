#include <iostream>
#include <vector>
#include "Engine.h"
#include "Input/InputManager.h"
#include "Graphics/GraphicsManager.h"
#include "ScriptManager/ScriptManager.h"
#include "Types.h"

int main(int argc, const char* argv[]) 
{
    willengine::Engine engine;

    engine.Startup({800, 600, "WillEngine - Sprite Demo", false});
    
    engine.script->LoadScript("test", "scripts/test.lua");
    engine.script->CallFunction("test", "init");

    engine.graphics->LoadTexture("player", "player.png");

    willengine::entityID player = engine.ecs.Create();

    // Add components to it
    engine.ecs.Get<willengine::Sprite>(player) = willengine::Sprite{
        .image = "player",
        .position = willengine::vec3(0, 0, 1),
        .scale = willengine::vec2(20.0f, 20.0f)
    };

    engine.ecs.Get<willengine::Velocity>(player) = willengine::Velocity(willengine::vec2(0, 0));
    engine.ecs.Get<willengine::Health>(player) = willengine::Health{ 100.0 };


    
    engine.RunGameLoop([&](){

        engine.script->CallFunction("test", "update");
        
    });
    
    engine.Shutdown();
    return 0;
}