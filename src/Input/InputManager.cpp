#include "InputManager.h"
#include "../Engine.h"
#include "../Graphics/GraphicsManager.h"
#include "spdlog/spdlog.h"


namespace willengine
{
	InputManager::InputManager(Engine* engine) : engine(engine)
	{
	}

	InputManager::~InputManager()
	{
	}
	void InputManager::Update()
	{
		glfwPollEvents();
	}
	bool InputManager::KeyIsPressed(Key key)
	{
		return glfwGetKey(engine->graphics->window, key) == GLFW_PRESS;
	}
}

