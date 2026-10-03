#include "brpch.h"
#include "LightSystem.h"

#include "Bruno/Scene/Components.h"
#include "Bruno/Scene/Scene.h"

namespace Bruno
{
    void LightSystem::Update(Scene* scene)
    {
        auto view = scene->GetAllEntitiesWith<TransformComponent, DirectionalLightComponent>();

        for (auto entity : view)
        {
            auto [transform, light] = view.get<TransformComponent, DirectionalLightComponent>(entity);

            // Si tu TransformComponent tiene un flag de "IsDirty", puedes optimizar
            // y recalcular esto SOLO si el transform de la luz se movió este frame.
            Math::Vector3 worldDir = Math::Vector3::TransformNormal(light.LocalDirection, transform.WorldTransform);
            worldDir.Normalize();

            light.WorldDirection = worldDir;
        }
    }
}
