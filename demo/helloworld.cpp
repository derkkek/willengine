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

    willengine::Engine engine{ willengine::Engine::Config{} };
    
    engine.RunGameLoop([&](){

        engine.script->CallFunction("test", "Update");
        
    });
    
    engine.Shutdown();
    return 0;
}