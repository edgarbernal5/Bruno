
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
        event Action<GizmoTransformable, Vector3> OnTranslationChanged;

        AxisGizmoTranslationRenderer AxisGizmoTranslationRenderer { get; }
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
        YZ,

        XYZ
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
