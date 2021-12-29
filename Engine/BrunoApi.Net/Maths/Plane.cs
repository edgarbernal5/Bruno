
using System;
using System.Runtime.InteropServices;

namespace BrunoApi.Net.Maths
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Plane : IEquatable<Plane>
    {
        public Vector3 Normal;
        public float D;

        public Plane(float a, float b, float c, float d)
        {
            Normal.X = a;
            Normal.Y = b;
            Normal.Z = c;
            D = d;
        }

        public Plane(Vector3 normal, float d)
        {
            Normal = normal;
            D = d;
        }

        public Plane(Vector4 value)
        {
            Normal.X = value.X;
            Normal.Y = value.Y;
            Normal.Z = value.Z;
            D = value.W;
        }

        public bool Equals(Plane other)
        {
            return Normal.X == other.Normal.X && Normal.Y == other.Normal.Y && Normal.Z == other.Normal.Z && D == other.D;
        }

        public override bool Equals(object obj)
        {
            bool result = false;
            if (obj is Plane)
            {
                result = Equals((Plane)obj);
            }
            return result;
        }

        public static bool operator ==(Plane value1, Plane value2)
        {
            return value1.Normal.X == value2.Normal.X && value1.Normal.Y == value2.Normal.Y && value1.Normal.Z == value2.Normal.Z && value1.D == value2.D;
        }

        public static bool operator !=(Plane value1, Plane value2)
        {
            return value1.Normal != value2.Normal || value1.D != value2.D;
        }

        public override int GetHashCode()
        {
            return Normal.GetHashCode() + D.GetHashCode();
        }

    }
}
