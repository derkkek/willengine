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

    //Load textures
    //engine.graphics->LoadTexture("player", "player.png");
    
    //sprites
    std::vector<willengine::Sprite> sprites = {
         {"player", willengine::vec3(0, 0, 0.5), willengine::vec2(10, 10)},
     };


    
    engine.RunGameLoop([&]() {
        //if (engine.input->KeyIsPressed(willengine::InputManager::Key::A)) 
        //{
        //    std::cout << "Key 'A' is pressed!\n";
        //}
        //if (engine.input->KeyIsPressed(willengine::InputManager::Key::ESC))
        //{
        //    engine.Shutdown();
        //}
        engine.script->CallFunction("test", "update");
        engine.graphics->Draw(sprites);
        
    });
    
    engine.Shutdown();
    return 0;
}