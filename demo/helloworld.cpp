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

    //engine.graphics->AddSprite("player", willengine::vec3(50, 0, 0.5), willengine::vec2(10, 10), "player.png");

    
    engine.RunGameLoop([&](){

        engine.script->CallFunction("test", "update");
        
    });
    
    engine.Shutdown();
    return 0;
}