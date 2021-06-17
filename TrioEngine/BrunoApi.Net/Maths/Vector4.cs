
using System;
using System.Runtime.InteropServices;

namespace BrunoApi.Net.Maths
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector4 : IEquatable<Vector4>
    {
        public float X;
        public float Y;
        public float Z;
        public float W;

        public Vector4(float _x, float _y, float _z)
        {
            X = _x;
            Y = _y;
            Z = _z;
            W = 0.0f;
        }

        public Vector4(float _x, float _y, float _z, float _w)
        {
            X = _x;
            Y = _y;
            Z = _z;
            W = _w;
        }

        public Vector4(float sameValue)
        {
            X = sameValue;
            Y = sameValue;
            Z = sameValue;
            W = sameValue;
        }

        public static bool operator ==(Vector4 value1, Vector4 value2)
        {
            return value1.X == value2.X && value1.Y == value2.Y && value1.Z == value2.Z && value1.W == value2.W;
        }

        public static bool operator !=(Vector4 value1, Vector4 value2)
        {
            return value1.X != value2.X || value1.Y != value2.Y || value1.Z != value2.Z || value1.W != value2.W;
        }

        public bool Equals(Vector4 other)
        {
            return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
        }

        public override bool Equals(object obj)
        {
            bool result = false;
            if (obj is Vector4)
            {
                result = Equals((Vector4)obj);
            }
            return result;
        }

        public override int GetHashCode()
        {
            return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode() + W.GetHashCode();
        }

        public override string ToString()
        {
            return $"X = {X}; Y = {Y}; Z = {Z}; W = {W}";
        }
    }
}
