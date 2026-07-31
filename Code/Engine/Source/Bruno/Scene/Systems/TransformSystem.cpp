#include "brpch.h"
#include "TransformSystem.h"

#include "Bruno/Scene/Scene.h"

namespace Bruno
{
    void TransformSystem::Update(Scene* scene)
    {
        // 1. Encontrar todas las entidades RAÍZ (sin padre)
        std::vector<entt::entity> rootEntities = GetRootEntities(scene);

        // 2. Actualizar desde la raíz hacia abajo recursivamente (o iterativamente)
        for (auto root : rootEntities)
        {
            Entity rootEntity { root, scene};
            UpdateTransformNode(scene, rootEntity, Math::Matrix::Identity, false);
        }
    }

    std::vector<entt::entity> TransformSystem::GetRootEntities(Scene* scene)
    {
        std::vector<entt::entity> roots;

        // Iteramos sobre el pool más grande que nos interesa: Transform
        auto view = scene->GetAllEntitiesWith<TransformComponent>();
            
        // Opcional: pre-reservar memoria para evitar saltos en el Heap.
        // view.size_hint() nos da una estimación basada en el array interno de EnTT.
        roots.reserve(view.size());

        for (auto entt : view)
        {
            Entity entity { entt, scene};
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

        // 1. Buscamos si la entidad tiene jerarquía
        const auto* hierarchy = scene->TryGetWith<HierarchyComponent>(entity);
        if (hierarchy)
        {
            // 2. Iteramos directamente sobre la lista enlazada intrusiva (¡Cero vectores!)
            entt::entity currentChild = hierarchy->FirstChild;
                
            while (currentChild != entt::null)
            {
                Entity childEntity { currentChild, scene};
                // Llamada recursiva al hijo
                UpdateTransformNode(scene, childEntity, transform.WorldTransform, needsUpdate);
                    
                // Saltamos al siguiente hermano para la próxima iteración del while
                currentChild = childEntity.GetComponent<HierarchyComponent>().NextSibling;
            }
        }
    }
}
