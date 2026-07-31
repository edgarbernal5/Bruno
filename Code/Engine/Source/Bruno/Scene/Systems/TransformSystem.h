#pragma once

#include <vector>
#include <entt/entt.hpp>

#include "Bruno/Math/Math.h"

namespace Bruno
{
    class Entity;
    class Scene;

    class TransformSystem
    {
    public:
        static void Update(Scene* scene);
        
    private:
        static std::vector<entt::entity> GetRootEntities(Scene* scene);
        static void UpdateTransformNode(Scene* scene, Entity entity, const Math::Matrix& parentWorld, bool parentIsDirty);
    };
}
