#include "PhysicsManager.h"
#include "../Engine.h"
namespace willengine
{
	PhysicsManager::PhysicsManager(Engine* engine)
		:engine(engine)
	{
	}
	void PhysicsManager::Update()
	{
		engine->ecs.ForEach<Rigidbody, Transform>([&](entityID entity) 
			{
				Rigidbody& rb = engine->ecs.Get<Rigidbody>(entity);
				Transform& transform = engine->ecs.Get<Transform>(entity);

				rb.position.x += rb.velocity.x;
				rb.position.y += rb.velocity.y;

				transform.x = rb.position.x;
				transform.y = rb.position.y;


			});
	}
}