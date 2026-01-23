#pragma once
#include <functional>
#include <string>
#include <glm/glm.hpp>
#include <typeindex>
#include <list>
#include <webgpu/webgpu.h>
namespace willengine
{
	class Engine;
	class GraphicsManager;
	class InputManager;
	class ResourceManager;
	class ScriptManager;
	class EventManager;
	class SoundManager;
	class PhysicsManager;
	class SceneManager;

	typedef std::function<void()> UpdateCallback;
	typedef std::function<void(WGPURenderPassEncoder)> RenderCallback;

	typedef glm::vec2 vec2;
	typedef glm::vec3 vec3;
	typedef glm::vec4 vec4;
	typedef glm::mat4 mat4;
	typedef long entityID;
	typedef std::type_index ComponentIndex;



	struct Rigidbody
	{
		vec2 position;
		vec2 velocity;

		Rigidbody() = default;
		Rigidbody(const vec2& pos, const vec2& vel) : position(pos), velocity(vel) {}
	};

	struct BoxCollider
	{
		vec2 dimensionSizes;
		bool isCollided;
		BoxCollider() = default;
		BoxCollider(const vec2& dimension_sizes, bool isCollided) : dimensionSizes(dimension_sizes), isCollided(isCollided = false){}
	};

	struct Sprite
	{
		std::string image;
		float alpha;
		vec2 scale;

		Sprite() = default;
		Sprite(const std::string& img, float a, const vec2& s) : image(img), alpha(a), scale(s) {}
	};

	struct Health
	{
		double percent;

		Health() = default;
		Health(double p) : percent(p) {}
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
	struct Script 
	{ 
		std::string name;
	};

}