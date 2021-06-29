
using BrunoFramework.Graphics;
using BrunoFramework.Graphics.Data;
using BrunoFramework.Graphics.Editor;
using System;
using BrunoApi.Net.Game;
using BrunoApi.Net.Maths;

namespace BrunoFramework.Editor.Game.Gizmos
{
    public interface IGizmoService
    {
        event Action<ITransformable, Vector3> OnTranslationChanged;
        event Action<ITransformable, Vector3, bool> OnScaleChanged;
        event Action<ITransformable, Quaternion> OnRotateChanged;

        AxisGizmoTranslationRenderer AxisGizmoTranslationRenderer { get; }
        Camera Camera { get; set; }

        bool IsActive { get; set; }
        GizmoType CurrentGizmo { get; set; }

        void SetTransformableSelected(ITransformable transformable);

        bool Begin(Vector2 mousePosition);
        void UpdateGizmo(Vector2 mousePosition);
        void SetGizmoAxis(Vector2 mousePosition);

        void SetSceneWorld(Matrix world);
        void Render(RenderContext renderContext);
        void End();
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
        ZX,
        YZ,

        XYZ
    }

    public enum PivotType
    {
        ObjectCenter,
        SelectionCenter,
        WorldOrigin
    }

    public interface ITransformable
    {
        Vector3 LocalPosition { get; set; }
        Vector3 LocalRotation { get; set; }
        Vector3 LocalScale { get; set; }
        Matrix WorldMatrix { get; set; }
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
