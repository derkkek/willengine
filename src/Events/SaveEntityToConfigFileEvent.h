#pragma once
#include "EventManager/EventManager.h"
namespace willengine
{
    struct EntitySaveData
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

	class SaveEntityToConfigFileEvent : public Event
	{
	public:
		SaveEntityToConfigFileEvent(const EntitySaveData& saveData)
            :saveData(saveData)
        {
        }
        EntitySaveData saveData;

    private:
	};
}