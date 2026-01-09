#pragma once
#include <functional>
namespace willengine
{
	class Engine;
	class GraphicsManager;
	class InputManager;
	typedef std::function<void()> UpdateCallback;
}