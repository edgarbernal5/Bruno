
using TrioApi.Net.Maths;

namespace EsteroFramework.Graphics.Data
{
    public class Camera
    {
        public Vector3 Position { get; set; }
        public Vector3 Target { get; set; }
        public Vector3 Up { get; set; }
        public float NearPlane { get; set; }
        public float FarPlane { get; set; }
        public float FieldOfView { get; set; }
        public float AspectRatio { get; set; }

        public float Distance { get; set; }
        public Matrix View { get; set; }
        public Quaternion Rotation { get; set; }

        public void Recalculate()
        {
            Vector3 translation = new Vector3(0.0f, 0.0f, Distance);
            translation = Vector3.Transform(translation, Rotation);

            Position = Target + translation;

            Up = Vector3.Transform(Vector3.Up, Rotation);

            View = Matrix.CreateLookAt(Position, Target, Up);
        }
    }
}
