#include "brpch.h"
#include "GizmoService.h"

#include "GizmoTranslationRenderer.h"
#include "GizmoRotationRenderer.h"
#include "Bruno/Editor/ObjectSelector.h"

#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Math/Math.h"
#include <limits>

namespace Bruno
{
    GizmoService::GizmoService(GraphicsDevice* device, Camera& camera, Surface* surface, ObjectSelector* objectSelector) :
        m_camera(camera),
        m_surface(surface),
        m_objectSelector(objectSelector)
    {        
        auto batch = std::make_shared<PrimitiveBatch<VertexPositionNormalColor>>(device, 4096 * 3*3, 4096*3);
        m_gizmoTranslationRenderer = std::make_shared<GizmoTranslationRenderer>(device, camera, surface, batch);
        m_gizmoRotationRenderer = std::make_shared<GizmoRotationRenderer>(device, camera, surface, batch);
        
        for (size_t i = 0; i < 3; i++)
        {
            m_activeAxisColors[i] = m_axisColors[i];
        }
        m_selectionState.m_rotationMatrix = Math::Matrix::Identity;
        m_selectionState.m_gizmoObjectOrientedWorld = Math::Matrix::Identity;
        m_selectionState.m_gizmoAxisAlignedWorld = Math::Matrix::Identity;
        m_selectionState.m_screenScaleMatrix = Math::Matrix::Identity;
        m_selectionState.m_objectRotation = Math::Matrix::Identity;
        m_selectionState.m_gizmoPosition = Math::Vector3::Zero;
        m_translationScaleSnapDelta = Math::Vector3::Zero;

        UpdateLocalState();
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

        Math::Vector3 intersectionPoint;
        if (GetAxisIntersectionPoint(mousePosition, intersectionPoint))
        {
            m_selectionState.m_prevMousePosition = mousePosition;
            m_selectionState.m_prevIntersectionPosition = intersectionPoint;
        }

        return true;
    }

    void GizmoService::Drag(const Math::Vector2& mousePosition)
    {
        switch (m_currentGizmoType)
        {
        case GizmoType::Translation:
        {
            auto translationDelta = GetDeltaMovement(mousePosition);
            translationDelta = ApplySnapAndPrecisionMode(translationDelta);

            translationDelta = Math::Vector3::Transform(translationDelta, m_selectionState.m_rotationMatrix);
            
            //TODO: se debe modificar el gizmoPosition siempre y no depender de los clientes (externos) por su posicion
            m_selectionState.m_gizmoPosition += translationDelta;
            BR_CORE_TRACE << "gizmo position x = " << m_selectionState.m_gizmoPosition.x << "; y = " << m_selectionState.m_gizmoPosition.y << "; z = " << m_selectionState.m_gizmoPosition.z << std::endl;

            if (m_dragTranslationCallback)
                m_dragTranslationCallback(translationDelta);
            
            break;
        }
        case GizmoType::Rotation:
        {
            auto rotationDelta = GetRotationDelta(mousePosition);
            //TODO: Apply snap and precision mode
            
            if(m_dragRotationCallback)
                m_dragRotationCallback(rotationDelta);

            break;
        }
        case GizmoType::Scale:
        {
            auto scaleDelta = GetDeltaMovement(mousePosition);
            scaleDelta = ApplySnapAndPrecisionMode(scaleDelta);

            if (m_dragScaleCallback)
                m_dragScaleCallback(scaleDelta);

            break;
        }
        }

        UpdateLocalState();
    }

    void GizmoService::OnMouseMove(const Math::Vector2& mousePosition)
    {
        auto selectedAxis = GetAxis(mousePosition);
        if (selectedAxis == GizmoAxis::None)
        {
            m_activeAxisColors[0] = m_axisColors[0];
            m_activeAxisColors[1] = m_axisColors[1];
            m_activeAxisColors[2] = m_axisColors[2];
        }
        else if (selectedAxis == GizmoAxis::XYZ)
        {
            m_activeAxisColors[0] = m_axisSelectionColor;
            m_activeAxisColors[1] = m_axisSelectionColor;
            m_activeAxisColors[2] = m_axisSelectionColor;
        }
        else if (selectedAxis == GizmoAxis::X || selectedAxis == GizmoAxis::Y || selectedAxis == GizmoAxis::Z)
        {
            int axisIndex = ((int)selectedAxis) - 1;
            m_activeAxisColors[axisIndex] = m_axisSelectionColor;
            m_activeAxisColors[(axisIndex + 1) % 3] = m_axisColors[(axisIndex + 1) % 3];
            m_activeAxisColors[(axisIndex + 2) % 3] = m_axisColors[(axisIndex + 2) % 3];
        }
        else if (selectedAxis == GizmoAxis::XY)
        {
            m_activeAxisColors[0] = m_axisSelectionColor;
            m_activeAxisColors[1] = m_axisSelectionColor;
            m_activeAxisColors[2] = m_axisColors[2];
        }
        else if (selectedAxis == GizmoAxis::YZ)
        {
            m_activeAxisColors[0] = m_axisColors[0];
            m_activeAxisColors[1] = m_axisSelectionColor;
            m_activeAxisColors[2] = m_axisSelectionColor;
        }
        else if (selectedAxis == GizmoAxis::XZ)
        {
            m_activeAxisColors[0] = m_axisSelectionColor;
            m_activeAxisColors[1] = m_axisColors[1];
            m_activeAxisColors[2] = m_axisSelectionColor;
        }
        switch (m_currentGizmoType)
        {
        case GizmoType::Translation:
        {
            m_gizmoTranslationRenderer->SetColors(m_activeAxisColors);
        }
        break;
        case GizmoType::Rotation:
        {
            m_gizmoRotationRenderer->SetColors(m_activeAxisColors);
        }
        break;
        }
    }

    void GizmoService::OnRender(GraphicsContext* context)
    {
        if (m_currentGizmoType == GizmoType::None)
            return;

        switch (m_currentGizmoType)
        {
        case Bruno::GizmoService::GizmoType::Translation:
            m_gizmoTranslationRenderer->Render(context);
            break;
        case Bruno::GizmoService::GizmoType::Rotation:
            m_gizmoRotationRenderer->Render(context);
            break;
        case Bruno::GizmoService::GizmoType::Scale:
            break;
        default:
            break;
        }
    }

    void GizmoService::Update()
    {
        if (m_currentGizmoType == GizmoType::None || !m_isActive)
            return;

        UpdateLocalState();

        switch (m_currentGizmoType)
        {
        case Bruno::GizmoService::GizmoType::Translation:
            m_gizmoTranslationRenderer->SetWorld(m_selectionState.m_gizmoObjectOrientedWorld);
            m_gizmoTranslationRenderer->Update();
            break;
        case Bruno::GizmoService::GizmoType::Rotation:
            m_gizmoRotationRenderer->SetWorld(m_selectionState.m_gizmoObjectOrientedWorld);
            m_gizmoRotationRenderer->Update();
            break;
        case Bruno::GizmoService::GizmoType::Scale:
            break;
        default:
            break;
        }
    }

    void GizmoService::UpdateLocalState()
    {
        float cameraDistance = GetCameraDistance();
        if (cameraDistance > 0.0f)
        {
            m_selectionState.m_screenScaleFactor = cameraDistance * Gizmo::GIZMO_SCREEN_SCALE;
        }
        else {
            m_selectionState.m_screenScaleFactor = 1.0f;
        }
        
        if (m_selectionState.m_screenScaleFactor < 0.0001f)
        {
            m_selectionState.m_invScreenScaleFactor = 1.0f;
        }
        else
        {
            m_selectionState.m_invScreenScaleFactor = 1.0f / m_selectionState.m_screenScaleFactor;
        }
        m_selectionState.m_screenScaleMatrix = Math::Matrix::CreateScale(m_selectionState.m_screenScaleFactor);

        if (m_objectSelector->GetSelectedObjects().size() == 0)
        {
            return;
        }
        m_selectionState.m_rotationMatrix = Math::Matrix::Identity;

        if (m_currentGizmoType == GizmoType::Translation ||
            m_currentGizmoType == GizmoType::Rotation)
        {
            auto world = Math::Matrix::Identity;

            if (m_transformSpace == TransformSpace::Local)
            {
                world = m_selectionState.m_objectRotation;
            }

            m_selectionState.m_gizmoObjectOrientedWorld = m_selectionState.m_screenScaleMatrix *
                Math::Matrix::CreateWorld(m_selectionState.m_gizmoPosition, world.Forward(), world.Up());

            m_selectionState.m_gizmoAxisAlignedWorld = m_selectionState.m_screenScaleMatrix *
                Math::Matrix::CreateWorld(m_selectionState.m_gizmoPosition, Math::Vector3::Forward, Math::Vector3::Up);

            m_selectionState.m_rotationMatrix.Forward(world.Forward());
            m_selectionState.m_rotationMatrix.Up(world.Up());
            m_selectionState.m_rotationMatrix.Right(world.Right());
        }
        else
        {
            //auto selectedObject = m_objectSelector->GetSelectedObjects()[0];
            auto localRotationMatrix = m_objectSelector->GetSelectedObjects()[0];
            auto localForward = localRotationMatrix.Forward();
            auto localUp = localRotationMatrix.Up();

            localForward.Normalize();
            auto localRight = localForward.Cross(localUp);
            localUp = localRight.Cross(localForward);
            localRight.Normalize();
            localUp.Normalize();

            m_selectionState.m_gizmoObjectOrientedWorld = m_selectionState.m_screenScaleMatrix *
                Math::Matrix::CreateWorld(m_selectionState.m_gizmoPosition, localForward, localUp);

            m_selectionState.m_gizmoAxisAlignedWorld = m_selectionState.m_gizmoObjectOrientedWorld;

            m_selectionState.m_rotationMatrix.Forward(localForward);
            m_selectionState.m_rotationMatrix.Up(localUp);
            m_selectionState.m_rotationMatrix.Right(localRight);
        }

    }

    void GizmoService::EndDrag()
    {
        m_selectionState.m_prevIntersectionPosition = Math::Vector3::Zero;
        m_selectionState.m_intersectionPosition = Math::Vector3::Zero;
        m_selectionState.m_prevMousePosition = Math::Vector2::Zero;
        m_currentDelta = Math::Vector3::Zero;
    }

    Math::Vector3 GizmoService::ApplySnapAndPrecisionMode(Math::Vector3 delta)
    {
        if (m_isSnapEnabled)
        {
            auto snapValue = m_currentGizmoType == GizmoType::Scale ?
                m_snapConfig.Scale :
                m_snapConfig.Translation;

            if (m_precisionModeEnabled)
            {
                delta *= m_snapConfig.PrecisionScale;
                snapValue *= m_snapConfig.PrecisionScale;
            }
            m_translationScaleSnapDelta += delta;

            delta = Math::Vector3((int)(m_translationScaleSnapDelta.x / snapValue) * snapValue,
                (int)(m_translationScaleSnapDelta.y / snapValue) * snapValue,
                (int)(m_translationScaleSnapDelta.z / snapValue) * snapValue);

            m_translationScaleSnapDelta -= delta;
        }
        else if (m_precisionModeEnabled)
        {
            delta *= m_snapConfig.PrecisionScale;
        }

        return delta;
    }

    void GizmoService::InitializeGizmos()
    {

    }

    Math::Vector3 GizmoService::GetDeltaMovement(const Math::Vector2& mousePosition)
    {
        Math::Vector3 delta = Math::Vector3::Zero;
        Math::Vector3 intersectionPoint;
        if (GetAxisIntersectionPoint(mousePosition, intersectionPoint))
        {
            m_selectionState.m_intersectionPosition = intersectionPoint;
            m_currentDelta = intersectionPoint - m_selectionState.m_prevIntersectionPosition;

            if (m_currentGizmoAxis == GizmoAxis::X || m_currentGizmoAxis == GizmoAxis::XY || m_currentGizmoAxis == GizmoAxis::XZ || m_currentGizmoAxis == GizmoAxis::XYZ)
            {
                delta.x = m_currentDelta.x;
            }
            if (m_currentGizmoAxis == GizmoAxis::Y || m_currentGizmoAxis == GizmoAxis::XY || m_currentGizmoAxis == GizmoAxis::YZ || m_currentGizmoAxis == GizmoAxis::XYZ)
            {
                delta.y = m_currentDelta.y;
            }
            if (m_currentGizmoAxis == GizmoAxis::Z || m_currentGizmoAxis == GizmoAxis::XZ || m_currentGizmoAxis == GizmoAxis::YZ || m_currentGizmoAxis == GizmoAxis::XYZ)
            {
                delta.z = m_currentDelta.z;
            }
        }

        m_selectionState.m_prevIntersectionPosition = m_selectionState.m_intersectionPosition;

        return delta;
    }

    Math::Quaternion GizmoService::GetRotationDelta(const Math::Vector2& mousePosition)
    {
        Math::Quaternion rotationDelta;

        if (m_currentGizmoAxis == GizmoAxis::XYZ)
        {
            auto gizmoScreenPosition = GetScreenPosition(m_selectionState.m_gizmoPosition);
            auto gizmoScreenPosition2 = GetScreenPosition(m_selectionState.m_gizmoPosition + m_camera.GetView().Right() * Gizmo::GIZMO_LENGTH);
            auto length = 3.0f * (gizmoScreenPosition2 - gizmoScreenPosition).Length() / DirectX::XM_2PI;
            Math::Vector2 deltaAngles(1.0f / length);

            Math::Vector2 mouseVelocity(mousePosition.x - m_selectionState.m_prevMousePosition.x, mousePosition.y - m_selectionState.m_prevMousePosition.y);

            auto angles = mouseVelocity * deltaAngles;

            rotationDelta = Math::Quaternion::CreateFromYawPitchRoll(angles.x, 0, 0) * Math::Quaternion::CreateFromYawPitchRoll(0, angles.y, 0);
        }
        else {
            Math::Vector3 planeNormals[3]{ m_selectionState.m_rotationMatrix.Right(), m_selectionState.m_rotationMatrix.Up(), m_selectionState.m_rotationMatrix.Forward()};
            int planeIndex = ((int)m_currentGizmoAxis) - 1;

            Math::Matrix gizmoWorldInverse = m_selectionState.m_gizmoAxisAlignedWorld.Invert();
            
            auto ray = ConvertMousePositionToRay(mousePosition);
            ray.position = Math::Vector3::Transform(ray.position, gizmoWorldInverse);
            ray.direction = Math::Vector3::TransformNormal(ray.direction, gizmoWorldInverse);
            ray.direction.Normalize();

            Math::Plane plane(planeNormals[planeIndex], 0);

            float delta = 0.0f;
            float intersection;
            if (ray.Intersects(plane, intersection))
            {
                auto positionOnPlane = ray.position + (ray.direction * intersection);

                auto newIntersectionPoint = positionOnPlane;
                newIntersectionPoint.Normalize();
                m_selectionState.m_intersectionPosition = newIntersectionPoint;

                float dotP = newIntersectionPoint.Dot(m_selectionState.m_prevIntersectionPosition);
                float acosAngle = Math::Clamp(dotP, -1.0f, 1.0f);
                float angle = Math::Acos(acosAngle);

                auto perpendicularVector = m_selectionState.m_prevIntersectionPosition;
                perpendicularVector.Cross(newIntersectionPoint);
                perpendicularVector.Normalize();

                float sign = perpendicularVector.Dot(planeNormals[planeIndex]);
                
                delta = sign * angle;
            }

            switch (m_currentGizmoAxis)
            {
            case GizmoAxis::X:
                rotationDelta *= Math::Quaternion::CreateFromAxisAngle(m_selectionState.m_rotationMatrix.Right(), delta);
                break;
            case GizmoAxis::Y:
                rotationDelta *= Math::Quaternion::CreateFromAxisAngle(m_selectionState.m_rotationMatrix.Up(), delta);
                break;
            case GizmoAxis::Z:
                rotationDelta *= Math::Quaternion::CreateFromAxisAngle(m_selectionState.m_rotationMatrix.Forward(), delta);
                break;
            }
        }

        return rotationDelta;
    }

    Math::Ray GizmoService::ConvertMousePositionToRay(const Math::Vector2& mousePosition)
    {
        Math::Vector3 nearPoint(mousePosition.x, mousePosition.y, 0.0f);
        Math::Vector3 farPoint(mousePosition.x, mousePosition.y, 1.0f);

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

        Math::Matrix gizmoWorldInverse = m_selectionState.m_gizmoObjectOrientedWorld.Invert();
        
        Math::Ray ray = ConvertMousePositionToRay(mousePosition);
        ray.position = Math::Vector3::Transform(ray.position, gizmoWorldInverse);
        ray.direction = Math::Vector3::TransformNormal(ray.direction, gizmoWorldInverse);
        ray.direction.Normalize();

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

                if (XYAxisBox.Intersects(ray.position, ray.direction, intersection)) {
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
                if (YZAxisBox.Intersects(ray.position, ray.direction, intersection)) {
                    if (intersection > closestIntersection)
                    {
                        selectedAxis = GizmoAxis::YZ;
                        closestIntersection = intersection;
                    }
                }
            }
            else if (m_currentGizmoType == GizmoType::Scale)
            {
                if (XYZAxisBox.Intersects(ray.position, ray.direction, intersection)) {
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
                    if (positionOnPlane.Length() > Gizmo::GIZMO_LENGTH)
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
                if (currentPointOnPlane.Length() < Gizmo::GIZMO_LENGTH * 0.8f)
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
        intersectionPoint = Math::Vector3::Zero;
        if (m_currentGizmoType == GizmoType::None || !m_isActive)
            return false;

        if (m_currentGizmoType == GizmoType::Translation || m_currentGizmoType == GizmoType::Scale)
        {
            auto gizmoWorldInverse = m_selectionState.m_rotationMatrix.Transpose();

            auto ray = ConvertMousePositionToRay(mousePosition);
            ray.position = Math::Vector3::Transform(ray.position, gizmoWorldInverse);
            ray.direction = Math::Vector3::TransformNormal(ray.direction, gizmoWorldInverse);
            ray.direction.Normalize();

            SetGizmoHandlePlaneFor(m_currentGizmoAxis, ray);

            float intersection;
            if (ray.Intersects(m_selectionState.m_currentGizmoPlane, intersection))
            {
                intersectionPoint = ray.position + (ray.direction * intersection);
                return true;
            }
        }
        else
        {
            if (m_currentGizmoAxis == GizmoAxis::XYZ)
            {
                return true;
            }

            Math::Vector3 planeNormals[3]{ m_selectionState.m_rotationMatrix.Right(), m_selectionState.m_rotationMatrix.Up(), m_selectionState.m_rotationMatrix.Forward() };

            int planeIndex = ((int)m_currentGizmoAxis) - 1;
            auto gizmoWorldInverse = m_selectionState.m_gizmoAxisAlignedWorld.Invert();

            auto ray = ConvertMousePositionToRay(mousePosition);
            ray.direction = Math::Vector3::TransformNormal(ray.direction, gizmoWorldInverse);
            ray.position = Math::Vector3::Transform(ray.position, gizmoWorldInverse);
            ray.direction.Normalize();

            Math::Plane plane(planeNormals[planeIndex], 0);

            float intersection;
            if (ray.Intersects(plane, intersection))
            {
                intersectionPoint = ray.position + (ray.direction * intersection);
                intersectionPoint.Normalize();
                return true;
            }
        }

        return false;
    }

    float GizmoService::GetCameraDistance()
    {
        if (m_camera.IsOrthographic()) {
            return 25.0f;
        }
        auto gizmoPositionViewSpace = Math::Vector3::Transform(m_selectionState.m_gizmoPosition, m_camera.GetView());
        if (Math::Abs(gizmoPositionViewSpace.z) > m_camera.GetNearPlane())
        {
            return Math::Abs(gizmoPositionViewSpace.z);
            //return m_camera.IsOrthographic() ? Math::Abs(gizmoPositionViewSpace.z) : gizmoPositionViewSpace.Length();
        }
        return 0.0f;
    }

    Math::Vector2 GizmoService::GetScreenPosition(const Math::Vector3& worldPosition)
    {
        auto point = m_camera.GetViewport().Project(worldPosition,
            m_camera.GetProjection(),
            m_camera.GetView(),
            Math::Matrix::Identity);

        return Math::Vector2(point.x, point.y);
    }

    void GizmoService::SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Vector2& mousePosition)
    {
        auto ray = ConvertMousePositionToRay(mousePosition);
        auto toLocal = m_selectionState.m_rotationMatrix.Invert();

        ray.position = Math::Vector3::Transform(ray.position, toLocal);
        ray.direction = Math::Vector3::TransformNormal(ray.direction, toLocal);

        SetGizmoHandlePlaneFor(selectedAxis, ray);
    }

    void GizmoService::SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Ray& ray)
    {
        auto toLocal = m_selectionState.m_rotationMatrix.Transpose();

        Math::Vector3 gizmoPositionInLocal = Math::Vector3::Transform(m_selectionState.m_gizmoPosition, toLocal);
        Math::Plane plane;
        Math::Vector3 planeNormal;
        float planeD = 0.0f;

        switch (selectedAxis)
        {
        case GizmoAxis::XY:
            planeNormal = Math::Vector3::Backward;
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