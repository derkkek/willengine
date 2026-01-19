#include <iostream>
#include <vector>
#include "Engine.h"
#include "InputManager/InputManager.h"
#include "GraphicsManager/GraphicsManager.h"
#include "ScriptManager/ScriptManager.h"
#include "EventManager/EventManager.h"
#include "SoundManager/SoundManager.h"
#include "Types.h"

int main(int argc, const char* argv[]) 
{
    willengine::Engine engine;

    engine.Startup({800, 600, "WillEngine - Sprite Demo", false});
    
    engine.script->LoadScript("test", "scripts/test.lua");
    engine.script->CallFunction("test", "start");
    
    engine.RunGameLoop([&](){

        engine.script->CallFunction("test", "update");
        
    });
    
    engine.Shutdown();
    return 0;
}