
namespace BrunoFramework.Editor.Game.Gizmos
{
    public class GizmoConfig
    {
        public float LINE_LENGTH = 3.0f;
        public float CONE_HEIGHT = 0.5f;
        public float CONE_RADIUS = 0.25f;
        public float LINE_OFFSET = 1.0f;

        public float GIZMO_LENGTH => LINE_LENGTH + CONE_HEIGHT;
    }

    public class GizmoSnappingConfig
    {
        public float TRANSLATION_SNAP = 1.0f;
        public float SCALE_SNAP = 0.5f;
        public float PRECISION_SCALE = 0.1f;
    }
}
