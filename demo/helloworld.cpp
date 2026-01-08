#include <iostream>
#include "Engine.h"
void up()
{

}
int main(int argc, const char* argv[]) {
    std::cout << "Hello, World!\n";
    willengine::Engine engine;
    // Use default config values
    engine.Startup({800,600, "hello", false});
    engine.RunGameLoop(up);
    engine.Shutdown();
    return 0;

}