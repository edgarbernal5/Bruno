
using EsteroFramework.Graphics;
using EsteroFramework.Graphics.Data;
using EsteroFramework.Graphics.Editor;
using System;
using TrioApi.Net.Game;
using TrioApi.Net.Maths;

namespace EsteroFramework.Editor.Game.Gizmos
{
    public interface IGizmoService
    {
        event Action<GizmoTransformable, Vector3> OnTranslationChanged;

        IAxisGizmoRenderer AxisGizmoRenderer { get; }
        Camera Camera { get; set; }

        bool IsActive { get; set; }
        GizmoType CurrentGizmo { get; set; }

        void SetObjectSelected(GizmoTransformable transform);

        bool Begin(Vector2 mousePosition);
        void UpdateGizmo(Vector2 mousePosition);

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
        YZ
    }

    public enum PivotType
    {
        ObjectCenter,
        SelectionCenter,
        WorldOrigin
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
