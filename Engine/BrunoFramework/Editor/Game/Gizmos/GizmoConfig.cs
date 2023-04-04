
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
        public float Translation { get; set; } = 1.0f;
        public float Scale { get; set; } = 0.5f;
        public float PrecisionScale { get; set; } = 0.1f;

        public GizmoSnappingConfig Clone()
        {
            var clone = new GizmoSnappingConfig();
            clone.Translation = Translation;
            clone.Scale = Scale;
            clone.PrecisionScale = PrecisionScale;
            return clone;
        }

        public void CopyFrom(GizmoSnappingConfig other)
        {
            Translation = other.Translation;
            Scale = other.Scale;
            PrecisionScale = other.PrecisionScale;
        }
    }
}
