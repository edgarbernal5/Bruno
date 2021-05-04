
using System.Runtime.InteropServices;
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

    }
}
