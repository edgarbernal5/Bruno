
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
}
