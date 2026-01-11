#pragma once
#include <functional>
#include <string>
#include <glm/glm.hpp>
namespace willengine
{
	class Engine;
	class GraphicsManager;
	class InputManager;
	class ResourceManager;
	typedef std::function<void()> UpdateCallback;
	typedef glm::vec2 vec2;
	typedef glm::vec3 vec3;
	typedef glm::vec4 vec4;
	typedef glm::mat4 mat4;

	struct Sprite
	{
		std::string image;
		vec3 position;  // x, y, z (z is for depth sorting)
		vec2 scale;     // width and height scale
	};

}