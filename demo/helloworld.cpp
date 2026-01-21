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

    willengine::Engine::Config engineConfig{ 800, 600, "WILL", false, 800.0f / 600.0f, 100.0f, 100.0f * 133.0f };

    willengine::Engine engine{ engineConfig };

    
    engine.script->LoadScript("test", "scripts/test.lua");
    engine.script->CallFunction("test", "start");
    
    engine.RunGameLoop([&](){

        engine.script->CallFunction("test", "update");
        
    });
    
    engine.Shutdown();
    return 0;
}