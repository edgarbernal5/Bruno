#pragma once

#include <entt/entt.hpp>

namespace Bruno
{
    class HierarchySystem
    {
    public:
        // Asigna un nuevo padre a una entidad
        static void SetParent(entt::registry& registry, entt::entity child, entt::entity newParent);
        
        // Desvincula a un hijo de su padre actual (lo deja en la raíz del mundo)
        static void Unparent(entt::registry& registry, entt::entity child);
        
        // Evita referencias circulares (Ej: que A sea padre de B, y B sea padre de A)
        static bool IsDescendantOf(entt::registry& registry, entt::entity child, entt::entity prospectiveParent);
    };
}
