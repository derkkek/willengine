#pragma once
#include <Types.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>

namespace willengine
{
    // Base class for sparse set holders
    class SparseSetHolder 
    {
    public:
        // A virtual destructor, since subclasses need their destructors to run to free memory.
        virtual ~SparseSetHolder() = default;
        virtual bool Has(entityID) const = 0;
        virtual void Drop(entityID) = 0;
    };

    // Subclasses are templated on the component type they hold.
    template<typename T>
    class SparseSet : public SparseSetHolder 
    {
    public:
        std::unordered_map<entityID, T> data;
        bool Has(entityID e) const override { return data.count(e) > 0; }
        void Drop(entityID e) override { data.erase(e); }
    };

    class ECS
    {
    public:
        ECS();
        ~ECS();

        // Create a new entity (returning its entity ID)
        entityID Create();

        // Destroy an entity, removing all of its components
        void Destroy(entityID e);

        // Get a given component for an entity. By returning a reference &, callers can also set the component
        template<typename T>
        T& Get(entityID entity) 
        {
            return GetAppropriateSparseSet<T>()[entity];
        }

        // Check if an entity has a given component
        template<typename T>
        bool Has(entityID entity) 
        {
            const ComponentIndex index = std::type_index(typeid(T));
            if (m_components[index] == nullptr) return false;
            return m_components[index]->Has(entity);
        }

        // Drop a component from an entity
        template<typename T>
        void Drop(entityID e) 
        {
            GetAppropriateSparseSet<T>().erase(e);
        }

        // Iterate over all entities with a given set of components and call a callback function
        typedef std::function<void(entityID)> ForEachCallback;

        template<typename EntitiesThatHaveThisComponent, typename... AndAlsoTheseComponents>
        void ForEach(const ForEachCallback& callback)
        {
            // Get a vector of ComponentIndex we can use with `m_components[index]->Has(entity)`.
            std::vector<ComponentIndex> also{ std::type_index(typeid(AndAlsoTheseComponents))... };

            // Iterate over entities in the first container.
            const ComponentIndex firstIndex = std::type_index(typeid(EntitiesThatHaveThisComponent));

            // Make sure the first component's sparse set exists
            if (m_components[firstIndex] == nullptr) return;

            // Get the sparse set for the first component type
            auto& sparseSet = GetAppropriateSparseSet<EntitiesThatHaveThisComponent>();

            // Iterate over all entities that have the first component
            for (const auto& [entity, component] : sparseSet) {
                // Check if the entity has all the other required components
                bool hasAllComponents = true;
                for (const ComponentIndex& idx : also) {
                    if (m_components[idx] == nullptr || !m_components[idx]->Has(entity)) {
                        hasAllComponents = false;
                        break;
                    }
                }

                // If the entity has all required components, call the callback
                if (hasAllComponents) {
                    callback(entity);
                }
            }
        }

    private:
        entityID m_nextID;
        std::unordered_map<ComponentIndex, std::unique_ptr<SparseSetHolder>> m_components;

        // Get the appropriate sparse set for a given component type
        template<typename T>
        std::unordered_map<entityID, T>& GetAppropriateSparseSet() 
        {
            // Get the index for T's SparseSet
            const ComponentIndex index = std::type_index(typeid(T));

            // Create the actual sparse set if needed.
            if (m_components[index] == nullptr) {
                m_components[index] = std::make_unique<SparseSet<T>>();
            }

            // It's safe to cast the SparseSetHolder to its subclass and return the std::unordered_map<entityID, T> inside.
            return static_cast<SparseSet<T>&>(*m_components[index]).data;
        }
    };
}