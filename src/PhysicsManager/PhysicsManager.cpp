#include "PhysicsManager.h"
#include "../Engine.h"
namespace willengine
{
	PhysicsManager::PhysicsManager(Engine* engine)
		:engine(engine), worldHalfHeight(0), worldHalfWidth(0)
	{

	}
    void PhysicsManager::Startup(Engine::Config& config)
    {
        worldHalfHeight = config.worldHalfHeight;
        worldHalfWidth = config.worldHalfWidth;
    }
    void PhysicsManager::Update()
    {



        engine->ecs.ForEach<Rigidbody, BoxCollider, Transform>([&](entityID entity)
            {
                Rigidbody& rb = engine->ecs.Get<Rigidbody>(entity);
                Transform& transform = engine->ecs.Get<Transform>(entity);
                BoxCollider& collider = engine->ecs.Get<BoxCollider>(entity);

                rb.position.x += rb.velocity.x;
                rb.position.y += rb.velocity.y;

                transform.x = rb.position.x;
                transform.y = rb.position.y;

                float halfSpriteWidth = collider.dimensionSizes.x;  // scale 20 = half-width 20
                float halfSpriteHeight = collider.dimensionSizes.y;

                // Right edge
                if (transform.x + halfSpriteWidth > worldHalfWidth)
                {
                    transform.x = worldHalfWidth - halfSpriteWidth;
                    rb.position.x = transform.x;
                    rb.velocity.x = 0;
                }
                // Left edge
                if (transform.x - halfSpriteWidth < -worldHalfWidth)
                {
                    transform.x = -worldHalfWidth + halfSpriteWidth;
                    rb.position.x = transform.x;
                    rb.velocity.x = 0;
                }
                if (transform.y + halfSpriteHeight > worldHalfHeight)
                {
                    transform.y = worldHalfHeight - halfSpriteHeight;
                    rb.position.y = transform.y;
                    rb.velocity.y = 0;
                }
                if (transform.y - halfSpriteHeight < -worldHalfHeight)
                {
                    transform.y = -worldHalfHeight + halfSpriteHeight;
                    rb.position.y = transform.y;
                    rb.velocity.y = 0;
                }
            });
    }
}