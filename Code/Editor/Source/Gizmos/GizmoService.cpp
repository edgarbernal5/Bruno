#include "GizmoService.h"

#include <limits>

namespace Bruno
{
    GizmoService::GizmoService(Camera& camera) :
        m_camera(camera)
    {
    }

    bool GizmoService::BeginDrag(const Math::Vector2& mousePosition)
    {
        if (m_currentGizmoType == GizmoType::None || !m_isActive)
            return false;

        auto selectedAxis = GetAxis(mousePosition);
        m_currentGizmoAxis = selectedAxis;
        m_currentDelta = Math::Vector3::Zero;

        if (m_currentGizmoAxis == GizmoAxis::None)
            return false;

        if (m_currentGizmoType == GizmoType::Translation || m_currentGizmoType == GizmoType::Scale)
            SetGizmoHandlePlaneFor(selectedAxis, mousePosition);

        return true;
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

    Math::Ray GizmoService::ConvertMousePositionToRay(const Math::Vector2& mousePosition)
    {
        Math::Vector3 nearPoint(mousePosition, 0.0f);
        Math::Vector3 farPoint(mousePosition, 1.0f);

        nearPoint = m_camera.GetViewport().Unproject(nearPoint,
            m_camera.GetProjection(),
            m_camera.GetView(),
            Math::Matrix::Identity);

        farPoint = m_camera.GetViewport().Unproject(farPoint,
            m_camera.GetProjection(),
            m_camera.GetView(),
            Math::Matrix::Identity);

        Math::Vector3 direction = farPoint - nearPoint;
        direction.Normalize();

        return Math::Ray(nearPoint, direction);
    }

    GizmoService::GizmoAxis GizmoService::GetAxis(const Math::Vector2& mousePosition)
    {
        float closestIntersection = (std::numeric_limits<float>::max)();
        auto selectedAxis = GizmoAxis::None;

        Math::Matrix gizmoWorldInverse = m_selectionState.m_gizmoObjectOrientedWorld;
        gizmoWorldInverse.Invert();

        Math::Ray ray = ConvertMousePositionToRay(mousePosition);
        ray.direction = Math::Vector3::TransformNormal(ray.direction, gizmoWorldInverse);
        ray.position = Math::Vector3::Transform(ray.position, gizmoWorldInverse);

        if (m_currentGizmoType == GizmoType::Translation || m_currentGizmoType == GizmoType::Scale)
        {
            float intersection = -1.0f;

            if (XAxisBox.Intersects(ray.position, ray.direction, intersection)) {
                if (intersection < closestIntersection)
                {
                    selectedAxis = GizmoAxis::X;
                    closestIntersection = intersection;
                }
            }
            if (YAxisBox.Intersects(ray.position, ray.direction, intersection)) {
                if (intersection < closestIntersection)
                {
                    selectedAxis = GizmoAxis::Y;
                    closestIntersection = intersection;
                }
            }
            if (ZAxisBox.Intersects(ray.position, ray.direction, intersection)) {
                if (intersection < closestIntersection)
                {
                    selectedAxis = GizmoAxis::Z;
                    closestIntersection = intersection;
                }
            }
            if (m_currentGizmoType == GizmoType::Translation)
            {
                if (closestIntersection >= (std::numeric_limits<float>::max)())
                    closestIntersection = (std::numeric_limits<float>::min)();

                if (XYBox.Intersects(ray.position, ray.direction, intersection)) {
                    if (intersection > closestIntersection)
                    {
                        selectedAxis = GizmoAxis::XY;
                        closestIntersection = intersection;
                    }
                }
                if (XZAxisBox.Intersects(ray.position, ray.direction, intersection)) {
                    if (intersection > closestIntersection)
                    {
                        selectedAxis = GizmoAxis::XZ;
                        closestIntersection = intersection;
                    }
                }
                if (YZBox.Intersects(ray.position, ray.direction, intersection)) {
                    if (intersection > closestIntersection)
                    {
                        selectedAxis = GizmoAxis::YZ;
                        closestIntersection = intersection;
                    }
                }
            }
            else if (m_currentGizmoType == GizmoType::Scale)
            {
                if (XYZBox.Intersects(ray.position, ray.direction, intersection)) {
                    if (intersection < closestIntersection)
                    {
                        selectedAxis = GizmoAxis::XYZ;
                        closestIntersection = intersection;
                    }
                }
            }
        }
        else if (m_currentGizmoType == GizmoType::Rotation)
        {
            Math::Vector3 planeNormals[3]{ Math::Vector3::Right, Math::Vector3::Up, Math::Vector3::Forward };

            auto currentPointOnPlane = Math::Vector3::Zero;
            for (int i = 0; i < 3; i++)
            {
                auto plane = Math::Plane(planeNormals[i], 0);
                float intersection;

                if (ray.Intersects(plane, intersection))
                {
                    auto positionOnPlane = ray.position + (ray.direction * intersection);
                    if (positionOnPlane.Length() > GIZMO_LENGTH)
                    {
                        continue;
                    }
                    else
                    {
                        if (intersection < closestIntersection)
                        {
                            currentPointOnPlane = positionOnPlane;
                            closestIntersection = intersection;

                            selectedAxis = (GizmoAxis)(i + 1);
                        }
                    }
                }
            }
            if (selectedAxis != GizmoAxis::None)
            {
                if (currentPointOnPlane.Length() < GIZMO_LENGTH * 0.8f)
                {
                    selectedAxis = GizmoAxis::XYZ;
                }
            }
        }

        if (closestIntersection >= (std::numeric_limits<float>::max)() || closestIntersection <= (std::numeric_limits<float>::min)())
            selectedAxis = GizmoAxis::None;

        return selectedAxis;
    }

    bool GizmoService::GetAxisIntersectionPoint(const Math::Vector2& mousePosition, Math::Vector3& intersectionPoint)
    {
        return false;
    }

    void GizmoService::SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Vector2& mousePosition)
    {
        auto ray = ConvertMousePositionToRay(mousePosition);
        auto toLocal = m_selectionState.m_rotationMatrix;
        toLocal.Invert();

        ray.position = Math::Vector3::Transform(ray.position, toLocal);
        ray.direction = Math::Vector3::TransformNormal(ray.direction, toLocal);

        SetGizmoHandlePlaneFor(selectedAxis, ray);
    }

    void GizmoService::SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Ray& ray)
    {
        auto toLocal = m_selectionState.m_rotationMatrix;
        toLocal.Transpose();

        Math::Vector3 gizmoPositionInLocal = Math::Vector3::Transform(m_selectionState.m_gizmoPosition, toLocal);
        Math::Plane plane;
        Math::Vector3 planeNormal;
        float planeD=0.0f;

        switch (selectedAxis)
        {
        case GizmoAxis::XY:
            planeNormal = Math::Vector3::Forward;
            planeD = gizmoPositionInLocal.z;
            break;
        case GizmoAxis::YZ:
            planeNormal = Math::Vector3::Left;
            planeD = gizmoPositionInLocal.x;
            break;
        case GizmoAxis::XZ:
            planeNormal = Math::Vector3::Down;
            planeD = gizmoPositionInLocal.y;
            break;

        case GizmoAxis::X:
        case GizmoAxis::Y:
        case GizmoAxis::Z:
        {
            auto cameraToGizmo = m_selectionState.m_gizmoPosition - m_camera.GetPosition();
            cameraToGizmo.Normalize();
            cameraToGizmo = Math::Vector3::TransformNormal(cameraToGizmo, toLocal);

            int axisIndex = (int)selectedAxis - 1;

            Math::Vector3 perpendicularRayVector;
            m_unaryDirections[axisIndex].Cross(cameraToGizmo, perpendicularRayVector);
            perpendicularRayVector.Cross(cameraToGizmo);
            
            perpendicularRayVector = m_unaryDirections[axisIndex].Cross(perpendicularRayVector);
            perpendicularRayVector.Normalize();

            float newD = -perpendicularRayVector.Dot(gizmoPositionInLocal);

            planeNormal = perpendicularRayVector;
            planeD = newD;
        }
        break;

        case GizmoAxis::XYZ:
        {
            auto cameraToGizmo = m_camera.GetPosition() - m_selectionState.m_gizmoPosition;
            cameraToGizmo = Math::Vector3::TransformNormal(cameraToGizmo, toLocal);

            float zCamera = cameraToGizmo.Length();
            cameraToGizmo.Normalize();

            planeNormal = cameraToGizmo;
            planeD = zCamera;
        }
        break;
        }

        //Logger.Debug(string.Format("selected plane normal = {0}; Axis = {1}", plane.Normal, selectedAxis.ToString()));
        m_selectionState.m_currentGizmoPlane = Math::Plane(planeNormal, planeD);
    }
}