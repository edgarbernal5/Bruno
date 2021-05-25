
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

        public Matrix View { get; set; }

        public Camera(Vector3 position, Vector3 target, Vector3 up)
        {
            Position = position;
            Target = target;
            Up = up;

            View = Matrix.CreateLookAt(position, target, up);
        }
    }
}
