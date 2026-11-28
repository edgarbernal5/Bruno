#include "brpch.h"
#include "TransformSystem.h"

#include "Bruno/Scene/Scene.h"

namespace Bruno
{
    void TransformSystem::Update(Scene* scene)
    {
        std::vector<entt::entity> rootEntities = GetRootEntities(scene);
        
        for (auto root : rootEntities)
        {
            Entity rootEntity { root, scene};
            UpdateTransformNode(scene, rootEntity, Math::Matrix::Identity, false);
        }
    }

    std::vector<entt::entity> TransformSystem::GetRootEntities(Scene* scene)
    {
        std::vector<entt::entity> roots;

        auto view = scene->GetAllEntitiesWith<TransformComponent>();
            
        // Opcional: pre-reservar memoria para evitar saltos en el Heap.
        roots.reserve(view.size());

        for (auto entt : view)
        {
            Entity entity { entt, scene };
            
            // try_get es O(1) puro. Devuelve un puntero crudo al componente o nullptr.
            const auto* hierarchy = scene->TryGetWith<HierarchyComponent>(entity);
                
            if (!hierarchy || hierarchy->Parent == entt::null)
            {
                roots.push_back(entt);
            }
        }

        return roots;
    }

    void TransformSystem::UpdateTransformNode(Scene* scene, Entity entity, const Math::Matrix& parentWorld, bool parentIsDirty)
    {
        auto& transform = entity.GetComponent<TransformComponent>();

        bool needsUpdate = transform.IsDirty || parentIsDirty;

        if (needsUpdate)
        {
            transform.LocalTransform = Math::Matrix::CreateScale(transform.Scale) * Math::Matrix::CreateFromQuaternion(transform.Rotation) * Math::Matrix::CreateTranslation(transform.Position);

            transform.WorldTransform = transform.LocalTransform * parentWorld;
            transform.IsDirty = false;
        }

        const auto* hierarchy = scene->TryGetWith<HierarchyComponent>(entity);
        if (hierarchy)
        {
            entt::entity currentChild = hierarchy->FirstChild;
                
            while (currentChild != entt::null)
            {
                Entity childEntity { currentChild, scene};

                UpdateTransformNode(scene, childEntity, transform.WorldTransform, needsUpdate);
                currentChild = childEntity.GetComponent<HierarchyComponent>().NextSibling;
            }
        }
    }
}
