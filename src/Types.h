#pragma once
#include <functional>
#include <string>
#include <glm/glm.hpp>
#include <typeindex>
namespace willengine
{
	class Engine;
	class GraphicsManager;
	class InputManager;
	class ResourceManager;
	class ScriptManager;

	typedef std::function<void()> UpdateCallback;
	typedef glm::vec2 vec2;
	typedef glm::vec3 vec3;
	typedef glm::vec4 vec4;
	typedef glm::mat4 mat4;
	typedef long entityID;
	typedef std::type_index ComponentIndex;

	struct Sprite
	{
		std::string image;
		vec3 position;  // x, y, z (z is for depth sorting)
		vec2 scale;     // width and height scale
	};

	struct Transform : public vec2
	{
		Transform() = default;
		Transform(const vec2& v) : vec2(v) {}
	};

	struct Velocity : public vec2
	{
		Velocity() = default;
		Velocity(const vec2& v) : vec2(v) {}
	};

	struct Gravity { double meters_per_second; };
	struct Health { double percent; };
	struct Script { std::string name; };

}