#include "ECS/ECS.h"

namespace willengine
{
    ECS::ECS() : m_nextID(0)
    {
    }

    ECS::~ECS()
    {
    }

    entityID ECS::Create()
    {
        m_nextID++;
        return m_nextID;
    }

    void ECS::Destroy(entityID e)
    {
        // Remove all components from this entity
        for (const auto& [index, comps] : m_components) {
            comps->Drop(e);
        }
    }
}