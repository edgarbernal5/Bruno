
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
        event Action<ITransformableObject, Vector3> OnTranslationChanged;
        event Action<ITransformableObject, Vector3, bool> OnScaleChanged;
        event Action<ITransformableObject, Quaternion> OnRotateChanged;

        AxisGizmoTranslationRenderer AxisGizmoTranslationRenderer { get; }
        Camera Camera { get; set; }

        bool IsActive { get; set; }
        GizmoType CurrentGizmo { get; set; }
        GizmoAxis CurrentGizmoAxis { get; }

        void SetTransformableAsSelected(IEditorObject editorObject);

        bool Begin(Vector2 mousePosition);
        void UpdateGizmo(Vector2 mousePosition);
        void SetGizmoAxisOverMousePosition(Vector2 mousePosition);

        void Render(RenderContext renderContext);
        void End();

        void SelectObjects(Vector2 mousePosition);
        ITransformableObject GetSelectedObject();
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

    public interface ITransformableObject
    {
        Vector3 LocalPosition { get; set; }
        Quaternion LocalRotation { get; set; }
        Vector3 LocalScale { get; set; }
        Matrix WorldMatrix { get; set; }
        Matrix ParentWorldMatrix { get; set; }
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
