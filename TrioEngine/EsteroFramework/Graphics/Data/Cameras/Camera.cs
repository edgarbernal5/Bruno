using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrioApi.Net.Maths;

namespace EsteroFramework.Graphics.Data
{
    public class Camera
    {
        public float FieldOfView { get; set; }

        public float NearPlane { get; set; }

        public float FarPlane { get; set; }

        public Vector3 Position { get; set; }
        public Vector3 Target { get; set; }
        public Vector3 Up { get; set; }


    }
}
