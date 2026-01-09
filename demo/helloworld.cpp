#include <iostream>
#include "Engine.h"
#include "Input/InputManager.h"
#include "Graphics/GraphicsManager.h"
int main(int argc, const char* argv[]) 
{
    std::cout << "Hello, World!\n";
    willengine::Engine engine;


    // Use default config values
    engine.Startup({800,600, "hello", false});
    engine.RunGameLoop([&]() {
        if (engine.input->KeyIsPressed(willengine::InputManager::Key::A)) 
        {
            std::cout << "Key 'A' is pressed!\n";
        }
        if (engine.input->KeyIsPressed(willengine::InputManager::Key::ESC))
        {
            engine.graphics->ShouldQuit();
            engine.Shutdown();
        }
        });
    engine.Shutdown();
    return 0;

}