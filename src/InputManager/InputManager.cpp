#include "InputManager.h"
#include "../Engine.h"
#include "../GraphicsManager/GraphicsManager.h"
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
		prevFrameKeyStates[W] = glfwGetKey(engine->graphics->window, W);
		prevFrameKeyStates[A] = glfwGetKey(engine->graphics->window, A);
		prevFrameKeyStates[S] = glfwGetKey(engine->graphics->window, S);
		prevFrameKeyStates[D] = glfwGetKey(engine->graphics->window, D);
		prevFrameKeyStates[ESC] = glfwGetKey(engine->graphics->window, ESC);

		glfwPollEvents();
	}
	bool InputManager::KeyIsPressedInFrame(Key key)
	{
		return glfwGetKey(engine->graphics->window, key) == GLFW_PRESS;
	}
	bool InputManager::KeyJustPressed(Key key)
	{
		int currentState = glfwGetKey(engine->graphics->window, key);
		return prevFrameKeyStates[key] == GLFW_RELEASE && currentState == GLFW_PRESS;
	}

	bool InputManager::KeyJustReleased(Key key)
	{
		int currentState = glfwGetKey(engine->graphics->window, key);
		return prevFrameKeyStates[key] == GLFW_PRESS && currentState == GLFW_RELEASE;
	}
}

