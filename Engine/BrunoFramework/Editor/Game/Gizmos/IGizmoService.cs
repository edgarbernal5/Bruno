
using BrunoFramework.Graphics;
using BrunoFramework.Graphics.Data;
using BrunoFramework.Graphics.Editor;
using System;
using BrunoApi.Net.Maths;
using BrunoApi.Net.Game.Components;

namespace BrunoFramework.Editor.Game.Gizmos
{
    public interface IGizmoService
    {
        event Action<ITransformableObject, Vector3> TranslationChanged;
        event Action<ITransformableObject, Vector3, bool> ScaleChanged;
        event Action<ITransformableObject, Quaternion> RotateChanged;

        AxisGizmoTranslationRenderer AxisGizmoTranslationRenderer { get; }
        Camera Camera { get; set; }

        bool IsActive { get; set; }
        GizmoType CurrentGizmo { get; set; }
        GizmoAxis CurrentGizmoAxis { get; }
        TransformSpace TransformSpace { get; set; }

        GizmoSnappingConfig SnappingConfig { get; }

        void SetEditorObjectAsSelected(IEditorObject editorObject);

        void UpdateSelectedObject(IEditorObject editorObject);

        bool BeginDrag(Vector2 mousePosition);
        void Drag(Vector2 mousePosition);
        void OnMouseMove(Vector2 mousePosition);

        void Render(RenderContext renderContext);
        void EndDrag();

        void SelectObjects(Vector2 mousePosition);
        ITransformableObject GetSelectedObject();

        bool SnapEnabled { get; set; }
        bool PrecisionModeEnabled { get; set; }
    }

    public enum GizmoType
    {
        None,
        Translation,
        Rotation,
        Scale
    }

    public enum GizmoAxis
    {
        None,
        X,
        Y,
        Z,
        XY,
        XZ,
        YZ,

        XYZ
    }

    public enum PivotType
    {
        ObjectCenter,
        SelectionCenter,
        WorldOrigin
    }

    public enum TransformSpace
    {
        Local,
        World
    }

    public interface ITransformableObject
    {
        Vector3 LocalPosition { get; set; }
        Quaternion LocalRotation { get; set; }
        Vector3 LocalScale { get; set; }
        Matrix WorldMatrix { get; set; }
    }

    public interface ISelectableObject
    {
        BoundingBox BoundingBox { get; }
        bool IsSelectable { get; }
        void SetObjectExtents(Vector3 center, Vector3 extents);
    }

    public interface IEditorObject : ITransformableObject, ISelectableObject
    {
    }

    public class GizmoTransformable
    {
        public long Id;
        public SceneTransform Transform;

        public GizmoTransformable(long id, SceneTransform sceneTransform)
        {
            Id = id;
            Transform = sceneTransform;
        }
    }
}
