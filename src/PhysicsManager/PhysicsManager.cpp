#include "PhysicsManager.h"
#include "../Engine.h"
namespace willengine
{
	PhysicsManager::PhysicsManager(Engine* engine)
		:engine(engine)
	{
		willengine::Engine::Config& config = engine->BringEngineConfiguration();
		worldWidth = 220.0f;
		worldHeight = worldWidth * (config.window_height / config.window_width);
	}
    void PhysicsManager::Update()
    {
        const Engine::Config& config = engine->BringEngineConfiguration();

        // Calculate actual world bounds (centered at 0)
        float aspectRatio = float(config.window_width) / float(config.window_height);
        float worldHalfHeight = 100.0f;  // From projection: 1/0.01 = 100
        float worldHalfWidth = worldHalfHeight * aspectRatio;  // ~133 for 800x600

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