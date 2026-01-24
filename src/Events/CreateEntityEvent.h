#pragma once
#include "EventManager/EventManager.h"
#include "Engine.h"
#include <string>
#include <optional>
namespace willengine
{
    struct EntityCreationData
    {
        std::string entityID;

        // Optional components - use std::optional to indicate presence
        std::optional<vec2> transform;

        std::optional<Rigidbody> rigidbody;

        std::optional<Sprite> sprite;

        std::optional<BoxCollider> boxCollider;

        std::optional<Health> health;

        std::optional<Script> script;
    };

    class CreateEntityEvent : public Event
    {

    public:
        CreateEntityEvent(const EntityCreationData& data)
            : entityData(data)
        {
        }

        EntityCreationData entityData;
    };
  
}