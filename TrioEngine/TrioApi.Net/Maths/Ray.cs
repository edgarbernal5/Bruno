
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Maths
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Ray : IEquatable<Ray>
    {
        public Vector3 Position;
        public Vector3 Direction;

        public Ray(Vector3 position, Vector3 direction)
        {
            Position = position;
            Direction = direction;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Ray_IntersectsPlane", CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool Internal_IntersectsPlane(ref Ray ray, ref Plane plane, ref float distance);


        public float? Intersects(Plane plane)
        {
            float distance = 0.0f;
            bool result = Internal_IntersectsPlane(ref this, ref plane, ref distance);

            return result ? new float?(distance) : null;
        }

        public bool Equals(Ray other)
        {
            return Position == other.Position && Direction == other.Direction;
        }

        public override bool Equals(object obj)
        {
            bool result = false;
            if (obj is Ray)
            {
                result = Equals((Ray)obj);
            }
            return result;
        }

        public static bool operator ==(Ray value1, Ray value2)
        {
            return value1.Position == value2.Position && value1.Direction == value2.Direction;
        }

        public static bool operator !=(Ray value1, Ray value2)
        {
            return value1.Position != value2.Position || value1.Direction != value2.Direction;
        }

        public override int GetHashCode()
        {
            return Position.GetHashCode() + Direction.GetHashCode();
        }
    }
}
