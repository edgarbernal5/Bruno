#include "GizmoService.h"

namespace Bruno
{
    GizmoService::GizmoService(Camera& camera) :
        m_camera(camera)
    {
    }

    bool GizmoService::BeginDrag(const Math::Vector2& mousePosition)
    {
        return false;
    }

    void GizmoService::Drag(const Math::Vector2& mousePosition)
    {
    }

    void GizmoService::OnMouseMove(const Math::Vector2& mousePosition)
    {
    }

    void GizmoService::Update()
    {
    }

    void GizmoService::EndDrag()
    {
    }

    void GizmoService::InitializeGizmos()
    {

    }

    GizmoService::GizmoAxis GizmoService::GetAxis(const Math::Vector2& mousePosition)
    {
        return GizmoAxis::None;
    }

    bool GizmoService::GetAxisIntersectionPoint(const Math::Vector2& mousePosition, Math::Vector3& intersectionPoint)
    {
        return false;
    }
}