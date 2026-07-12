#pragma once

#include "Bruno/Math/Math.h"

#include "Bruno/Renderer/PrimitiveBatch_Gem.h"
#include "Gizmos/GizmoConfig.h"

namespace Bruno::DX
{
    class GraphicsDevice;
    class GraphicsContext;
    class GraphicsPipelineState;
    class RootSignature;
    class Shader;

    // Constantes de transformación (Se pasan al Root Signature del Shader de Gizmos)
    struct GizmoConstants
    {
        Math::Matrix ViewProjection;
    };
    
    class GizmoService
    {
    public:
        enum class GizmoType
        {
            None,
            Translation,
            Rotation,
            Scale
        };
		
        enum class GizmoAxis
        {
            None,
            X,
            Y,
            Z,
            XY,
            XZ,
            YZ,

            XYZ
        };
		
        enum class PivotType
        {
            ObjectCenter,
            SelectionCenter,
            WorldOrigin
        };
		
        enum class TransformSpace
        {
            Local,
            World
        };

        struct SnapConfig
        {
            float Translation{ 1.0f };
            float Scale{ 0.5f };
            float PrecisionScale{ 0.1f };
        };
        
        using DragTranslationCallback = std::function<void(const Math::Vector3&)>;
        using DragScaleCallback = std::function<void(const Math::Vector3&, bool isUniform)>;
        using DragRotationCallback = std::function<void(const Math::Quaternion&)>;
        
    public:
        GizmoService(GraphicsDevice* device, Camera& camera);
        void Initialize();
    
        void BuildGeometry(uint32_t frameIndex);
        
        // Prepara buffers y realiza el render final en la GPU
        void Render(DX::GraphicsContext* context, uint32_t frameIndex, const Math::Matrix& viewProjection);
        
        // Interacción
        bool BeginDrag(const Math::Vector2& mousePosition);
		void Drag(const Math::Vector2& mousePosition);
        void EndDrag();
        void Update();
        void OnMouseMove(const Math::Vector2& mousePosition);
        
        TransformSpace GetTransformSpace() const { return m_transformSpace; }
        
        void SetTranslationCallback(DragTranslationCallback callback) { m_dragTranslationCallback = std::move(callback); }
        void SetScaleCallback(DragScaleCallback callback) { m_dragScaleCallback = std::move(callback); }
        void SetRotationCallback(DragRotationCallback callback) { m_dragRotationCallback = std::move(callback); }

        bool IsDragging() const { return m_selectionState.m_isDragging; }
        void SetGizmoType(GizmoType type);
        void SetTransformSpace(TransformSpace space);
		void SetActive(bool isActive){ m_isActive = isActive; }
        
        void SetGizmoPosition(const Math::Vector3& position);
        void SetGizmoWorldMatrix(const Math::Matrix& worldTransform);
        
    private:
        struct SelectionState
        {
            Math::Vector3 m_gizmoPosition;
            Math::Vector3 m_initialGizmoPosition;
            Math::Matrix m_gizmoWorld;
            Math::Matrix m_initialGizmoWorld;
            
            float m_screenScaleFactor;
            Math::Matrix m_screenScaleMatrix;

            Math::Matrix m_gizmoObjectOrientedWorld;
            Math::Matrix m_gizmoAxisAlignedWorld;

            Math::Matrix m_rotationMatrix;
            Math::Quaternion m_cameraViewInverseRotation;
            Math::Quaternion m_cameraViewInverseRotationConjugate;

            Math::Vector3 m_intersectionPosition, m_prevIntersectionPosition;
            Math::Vector2 m_prevMousePosition;

            Math::Plane m_currentGizmoPlane;
            bool m_isDragging;
        };
        
        void BuildGizmoGeometry(uint32_t frameIndex);
        void BuildCameraGizmoGeometry(uint32_t frameIndex);
        
        void RenderBatch(DX::GraphicsContext* context, DX::PrimitiveBatch& primitiveBatch, uint32_t frameIndex, const Math::Matrix& viewProjection);
        
        void RenderCameraGizmo(GraphicsContext* context, uint32_t frameIndex, Math::Viewport mainViewport);
        
        GizmoAxis GetAxis(const Math::Vector2& mousePosition);
        
        Math::Vector2 GetScreenPosition(const Math::Vector3& worldPosition);
        Math::Ray ConvertMousePositionToRay(const Math::Vector2& mousePosition);
        bool GetAxisIntersectionPoint(const Math::Vector2& mousePosition, Math::Vector3& intersectionPoint);
        
        Math::Quaternion GetRotationDelta(const Math::Vector2& mousePosition);
        Math::Vector3 GetDeltaMovement(const Math::Vector2& mousePosition);
        Math::Vector3 ConstrainToAxis(const Math::Vector3& movement, GizmoAxis axis);
        Math::Vector3 ApplySnapAndPrecisionMode(Math::Vector3 delta);
        float GetCameraDistance() const;
        
        void UpdateLocalState();
        
        void SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Vector2& mousePosition);
        void SetGizmoHandlePlaneForRotation(GizmoAxis selectedAxis, const Math::Vector2& mousePosition);

        void SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, const Math::Ray& ray);
        
        const Math::BoundingBox XAxisBox
        {
            DirectX::XMFLOAT3((Gizmo::GIZMO_LENGTH + Gizmo::LINE_OFFSET) * 0.5f, 0.0f, 0.0f),
            DirectX::XMFLOAT3((Gizmo::GIZMO_LENGTH - Gizmo::LINE_OFFSET) * 0.5f, Gizmo::SINGLE_AXIS_THICKNESS * 0.5f, Gizmo::SINGLE_AXIS_THICKNESS * 0.5f)
        };

        const Math::BoundingBox YAxisBox
        {
            DirectX::XMFLOAT3(0.0f, (Gizmo::GIZMO_LENGTH + Gizmo::LINE_OFFSET) * 0.5f, 0.0f),
            DirectX::XMFLOAT3(Gizmo::SINGLE_AXIS_THICKNESS * 0.5f, (Gizmo::GIZMO_LENGTH - Gizmo::LINE_OFFSET) * 0.5f, Gizmo::SINGLE_AXIS_THICKNESS * 0.5f)
        };

        const Math::BoundingBox ZAxisBox
        {
            DirectX::XMFLOAT3(0.0f, 0.0f, (Gizmo::GIZMO_LENGTH + Gizmo::LINE_OFFSET) * 0.5f),
            DirectX::XMFLOAT3(Gizmo::SINGLE_AXIS_THICKNESS * 0.5f, Gizmo::SINGLE_AXIS_THICKNESS * 0.5f, (Gizmo::GIZMO_LENGTH - Gizmo::LINE_OFFSET) * 0.5f)
        };

        const Math::BoundingBox XZAxisBox
        {
            DirectX::XMFLOAT3(Gizmo::LINE_OFFSET * 0.5f, Gizmo::MULTI_AXIS_THICKNESS * 0.5f, Gizmo::LINE_OFFSET * 0.5f),
            DirectX::XMFLOAT3(Gizmo::LINE_OFFSET * 0.5f, Gizmo::MULTI_AXIS_THICKNESS * 0.5f, Gizmo::LINE_OFFSET * 0.5f)
        };

        const Math::BoundingBox XYAxisBox
        {
            DirectX::XMFLOAT3(Gizmo::LINE_OFFSET * 0.5f, Gizmo::LINE_OFFSET * 0.5f, Gizmo::MULTI_AXIS_THICKNESS * 0.5f),
            DirectX::XMFLOAT3(Gizmo::LINE_OFFSET * 0.5f, Gizmo::LINE_OFFSET * 0.5f, Gizmo::MULTI_AXIS_THICKNESS * 0.5f)
        };

        const Math::BoundingBox YZAxisBox
        {
            DirectX::XMFLOAT3(Gizmo::MULTI_AXIS_THICKNESS * 0.5f, Gizmo::LINE_OFFSET * 0.5f, Gizmo::LINE_OFFSET * 0.5f),
            DirectX::XMFLOAT3(Gizmo::MULTI_AXIS_THICKNESS * 0.5f, Gizmo::LINE_OFFSET * 0.5f, Gizmo::LINE_OFFSET * 0.5f)
        };

        const Math::BoundingBox XYZAxisBox
        {
            DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
            DirectX::XMFLOAT3(Gizmo::LINE_LENGTH * 0.25f, Gizmo::LINE_LENGTH * 0.25f, Gizmo::LINE_LENGTH * 0.25f)
        };
        
        PrimitiveBatch m_primitiveBatch; // Para el Gizmo 3D (Traslación, Rotación, Escalado)
        PrimitiveBatch m_cameraGizmoBatch; // Para la brújula de la esquina
        
        const Math::Vector3 m_unaryDirections[3]{ Math::Vector3::UnitX, Math::Vector3::UnitY, Math::Vector3::UnitZ };

        Math::Color m_axisColors[3]{ Math::Color(1.0f,0.0f,0.0f,1.0f),Math::Color(0.0f,1.0f,0.0f,1.0f), Math::Color(0.0f,0.0f,1.0f,1.0f) };
        Math::Color m_axisSelectionColor = Math::Color(0.5f, 0.5f, 0.25f, 1);
        
        GraphicsDevice* m_device;
        Camera& m_camera;
        Camera m_sceneGizmoCamera;
        
        SelectionState m_selectionState{};
        
        DragTranslationCallback m_dragTranslationCallback;
        DragScaleCallback m_dragScaleCallback;
        DragRotationCallback m_dragRotationCallback;
        
        bool m_isActive{ false };
        GizmoType m_currentGizmoType = GizmoType::Translation;
        PivotType m_pivotType = PivotType::SelectionCenter;
        GizmoAxis m_currentAxis = GizmoAxis::None;
        TransformSpace m_transformSpace = TransformSpace::Local;
        GizmoConfig m_gizmoConfig;
        
        std::unique_ptr<DX::RootSignature> m_rootSignature;
        std::unique_ptr<GraphicsPipelineState> m_psoDepthOn;  // Gizmos con profundidad (normal)
        std::unique_ptr<GraphicsPipelineState> m_psoDepthOff; // Gizmos "X-Ray" dibujados sobre los objetos

        std::unique_ptr<DX::Shader> m_vertexShader;
        std::unique_ptr<DX::Shader> m_pixelShader;
    };
}
