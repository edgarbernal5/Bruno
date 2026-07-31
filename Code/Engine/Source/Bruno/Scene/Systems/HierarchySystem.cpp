#include "brpch.h"
#include "HierarchySystem.h"

#include "Bruno/Scene/Components.h"

namespace Bruno
{
    void HierarchySystem::SetParent(entt::registry& registry, entt::entity child, entt::entity newParent)
    {
        // 1. Validaciones básicas
        if (child == newParent || child == entt::null) return;

        // 2. Evitar ciclos infinitos en el árbol
        if (IsDescendantOf(registry, newParent, child)) return;

        // 3. Remover de la jerarquía anterior si ya tenía un padre
        Unparent(registry, child);

        // Si el objetivo era simplemente quitarle el padre, terminamos.
        if (newParent == entt::null) return;

        // 4. Asegurar que ambas entidades tengan el componente (C++17 feature de EnTT)
        auto& childHierarchy = registry.get_or_emplace<HierarchyComponent>(child);
        auto& parentHierarchy = registry.get_or_emplace<HierarchyComponent>(newParent);

        // 5. Inserción al principio de la lista del nuevo padre (O(1) ultra rápido)
        childHierarchy.Parent = newParent;
        childHierarchy.NextSibling = parentHierarchy.FirstChild;
        childHierarchy.PrevSibling = entt::null;

        if (parentHierarchy.FirstChild != entt::null)
        {
            auto& oldFirstChild = registry.get<HierarchyComponent>(parentHierarchy.FirstChild);
            oldFirstChild.PrevSibling = child;
        }

        parentHierarchy.FirstChild = child;

        // 6. Opcional: Marcar el Transform como sucio para que se recalcule este frame
        if (auto* transform = registry.try_get<TransformComponent>(child))
        {
            transform->IsDirty = true;
        }
    }

    void HierarchySystem::Unparent(entt::registry& registry, entt::entity child)
    {
        if (child == entt::null) return;

        // Si no tiene componente o no tiene padre, no hay nada que hacer
        auto* childHierarchy = registry.try_get<HierarchyComponent>(child);
        if (!childHierarchy || childHierarchy->Parent == entt::null) return;

        entt::entity parent = childHierarchy->Parent;
        auto& parentHierarchy = registry.get<HierarchyComponent>(parent);

        // 1. Si el hijo tiene un hermano anterior, unimos ese hermano con el siguiente
        if (childHierarchy->PrevSibling != entt::null)
        {
            auto& prevNode = registry.get<HierarchyComponent>(childHierarchy->PrevSibling);
            prevNode.NextSibling = childHierarchy->NextSibling;
        }
        else
        {
            // Si no tiene hermano previo, significa que era el FirstChild del padre
            parentHierarchy.FirstChild = childHierarchy->NextSibling;
        }

        // 2. Si el hijo tiene un hermano siguiente, unimos ese hermano con el anterior
        if (childHierarchy->NextSibling != entt::null)
        {
            auto& nextNode = registry.get<HierarchyComponent>(childHierarchy->NextSibling);
            nextNode.PrevSibling = childHierarchy->PrevSibling;
        }

        // 3. Limpiar referencias del hijo independizado
        childHierarchy->Parent = entt::null;
        childHierarchy->NextSibling = entt::null;
        childHierarchy->PrevSibling = entt::null;

        // 4. Marcar transform como sucio (ahora está en espacio de mundo)
        if (auto* transform = registry.try_get<TransformComponent>(child))
        {
            transform->IsDirty = true;
        }
    }

    bool HierarchySystem::IsDescendantOf(entt::registry& registry, entt::entity child, entt::entity prospectiveParent)
    {
        if (child == entt::null || prospectiveParent == entt::null) return false;
        
        // Subimos por el árbol verificando si encontramos al 'prospectiveParent'
        entt::entity current = child;
        while (current != entt::null)
        {
            if (current == prospectiveParent) return true;
            
            auto* hierarchy = registry.try_get<HierarchyComponent>(current);
            if (!hierarchy) break;
            
            current = hierarchy->Parent;
        }
        return false;
    }
}
