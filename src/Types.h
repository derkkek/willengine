#pragma once
#include <functional>
#include <glm/glm.hpp>
namespace willengine
{
	class Engine;
	class GraphicsManager;
	class InputManager;
	typedef std::function<void()> UpdateCallback;
	typedef glm::vec2 vec2;
	typedef glm::vec3 vec3;
	typedef glm::vec4 vec4;
	typedef glm::mat4 mat4;

}