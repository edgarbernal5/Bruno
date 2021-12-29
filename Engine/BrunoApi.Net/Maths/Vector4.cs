
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

        private static Vector4 g_zero;
        private static Vector4 g_one;

        public static Vector4 Zero
        {
            get
            {
                return g_zero;
            }
        }
        public static Vector4 One
        {
            get
            {
                return g_one;
            }
        }
        static Vector4()
        {
            g_zero = new Vector4();
            g_one = new Vector4(1f);
        }

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

        public Vector4(Vector3 xyz, float _w)
        {
            X = xyz.X;
            Y = xyz.Y;
            Z = xyz.Z;
            W = _w;
        }

        public Vector4(float sameValue)
        {
            X = sameValue;
            Y = sameValue;
            Z = sameValue;
            W = sameValue;
        }


        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector4_TransformMatrixPosition", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_TransformMatrix(ref Vector4 up, ref Matrix matrix);

        public static Vector4 Transform(Vector4 position, Matrix matrix)
        {
            Internal_TransformMatrix(ref position, ref matrix);
            return position;
        }

        //[DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector4_TransformMatrixNormal", CallingConvention = CallingConvention.StdCall)]
        //private static extern void Internal_TransformMatrixNormal(ref Vector4 vector, ref Matrix matrix);

        //public static Vector4 TransformVector(Vector4 vector, Matrix matrix)
        //{
        //    Internal_TransformMatrixNormal(ref vector, ref matrix);
        //    return vector;
        //}

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

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector4_SubTwoVectors", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SubTwoVectors(ref Vector4 vector, ref Vector4 vector2);

        public static Vector4 operator -(Vector4 vector1, Vector4 vector2)
        {
            Internal_SubTwoVectors(ref vector1, ref vector2);
            return vector1;
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
