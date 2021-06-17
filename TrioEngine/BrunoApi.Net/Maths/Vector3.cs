
using System;
using System.Runtime.InteropServices;

namespace BrunoApi.Net.Maths
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3 : IEquatable<Vector3>
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_Clamp", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Clamp(ref Vector3 vector, ref Vector3 vMin, ref Vector3 vMax);

        public float X;
        public float Y;
        public float Z;

        private static Vector3 g_zero;
        private static Vector3 g_one;
        private static Vector3 g_unitX;
        private static Vector3 g_unitY;
        private static Vector3 g_unitZ;
        private static Vector3 g_up;
        private static Vector3 g_down;
        private static Vector3 g_right;
        private static Vector3 g_left;
        private static Vector3 g_forward;
        private static Vector3 g_backward;

        public static Vector3 Zero
        {
            get
            {
                return g_zero;
            }
        }
        public static Vector3 One
        {
            get
            {
                return g_one;
            }
        }
        public static Vector3 UnitX
        {
            get
            {
                return g_unitX;
            }
        }
        public static Vector3 UnitY
        {
            get
            {
                return g_unitY;
            }
        }
        public static Vector3 UnitZ
        {
            get
            {
                return g_unitZ;
            }
        }
        public static Vector3 Up
        {
            get
            {
                return g_up;
            }
        }
        public static Vector3 Down
        {
            get
            {
                return g_down;
            }
        }
        public static Vector3 Right
        {
            get
            {
                return g_right;
            }
        }
        public static Vector3 Left
        {
            get
            {
                return g_left;
            }
        }
        public static Vector3 Forward
        {
            get
            {
                return g_forward;
            }
        }
        public static Vector3 Backward
        {
            get
            {
                return g_backward;
            }
        }

        static Vector3()
        {
            g_zero = new Vector3();
            g_one = new Vector3(1f, 1f, 1f);
            g_unitX = new Vector3(1f, 0f, 0f);
            g_unitY = new Vector3(0f, 1f, 0f);
            g_unitZ = new Vector3(0f, 0f, 1f);
            g_up = new Vector3(0f, 1f, 0f);
            g_down = new Vector3(0f, -1f, 0f);
            g_right = new Vector3(1f, 0f, 0f);
            g_left = new Vector3(-1f, 0f, 0f);
            g_forward = new Vector3(0f, 0f, -1f);
            g_backward = new Vector3(0f, 0f, 1f);
        }

        public Vector3(float _x, float _y, float _z)
        {
            X = _x;
            Y = _y;
            Z = _z;
        }

        public Vector3(Vector2 _xy, float _z)
        {
            X = _xy.X;
            Y = _xy.Y;
            Z = _z;
        }

        public Vector3(float sameValue)
        {
            X = sameValue;
            Y = sameValue;
            Z = sameValue;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_Cross", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Cross(ref Vector3 vector, ref Vector3 vector2);

        public static Vector3 Cross(Vector3 vector1, Vector3 vector2)
        {
            Internal_Cross(ref vector1, ref vector2);
            return vector1;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_Distance", CallingConvention = CallingConvention.StdCall)]
        private static extern float Internal_Distance(ref Vector3 vector, ref Vector3 vector2);

        public static float Distance(Vector3 value1, Vector3 value2)
        {
            return Internal_Distance(ref value1, ref value2);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_DistanceSquared", CallingConvention = CallingConvention.StdCall)]
        private static extern float Internal_DistanceSquared(ref Vector3 vector, ref Vector3 vector2);

        public static float DistanceSquared(Vector3 value1, Vector3 value2)
        {
            return Internal_DistanceSquared(ref value1, ref value2);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_Dot", CallingConvention = CallingConvention.StdCall)]
        private static extern float Internal_Dot(ref Vector3 vector, ref Vector3 vector2);

        public static float Dot(Vector3 vector1, Vector3 vector2)
        {
            return Internal_Dot(ref vector1, ref vector2);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_Length", CallingConvention = CallingConvention.StdCall)]
        private static extern float Internal_Cross(ref Vector3 vector);

        public float Length()
        {
            return Internal_Cross(ref this);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_Normalize", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Normalize(ref Vector3 vector);

        public static Vector3 Normalize(Vector3 vector)
        {
            Internal_Normalize(ref vector);
            return vector;
        }

        public void Normalize()
        {
            Internal_Normalize(ref this);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_TransformQuat", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_TransformQuat(ref Vector3 up, ref Quaternion quaternion);

        public static Vector3 Transform(Vector3 up, Quaternion rotation)
        {
            Internal_TransformQuat(ref up, ref rotation);
            return up;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_TransformMatrixPosition", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_TransformMatrix(ref Vector3 up, ref Matrix matrix);

        public static Vector3 Transform(Vector3 position, Matrix matrix)
        {
            Internal_TransformMatrix(ref position, ref matrix);
            return position;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_TransformMatrixNormal", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_TransformMatrixNormal(ref Vector3 normal, ref Matrix matrix);

        public static Vector3 TransformNormal(Vector3 normal, Matrix matrix)
        {
            Internal_TransformMatrixNormal(ref normal, ref matrix);
            return normal;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_UnaryNegation", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_UnaryNegation(ref Vector3 vector);

        public static Vector3 operator -(Vector3 vectorIn)
        {
            Internal_UnaryNegation(ref vectorIn);
            return vectorIn;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_SumTwoVectors", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SumTwoVectors(ref Vector3 vector, ref Vector3 vector2);

        public static Vector3 operator +(Vector3 vector1, Vector3 vector2)
        {
            Internal_SumTwoVectors(ref vector1, ref vector2);
            return vector1;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_SubTwoVectors", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SubTwoVectors(ref Vector3 vector, ref Vector3 vector2);

        public static Vector3 operator -(Vector3 vector1, Vector3 vector2)
        {
            Internal_SubTwoVectors(ref vector1, ref vector2);
            return vector1;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_MultiplyScalar", CallingConvention = CallingConvention.StdCall)]
        private static extern float Internal_MultiplyScalar(ref Vector3 vector, float scalar);

        public static Vector3 operator *(Vector3 vector1, float scaleFactor)
        {
            Internal_MultiplyScalar(ref vector1, scaleFactor);
            return vector1;
        }

        public static Vector3 operator *(float scaleFactor, Vector3 vector1)
        {
            Internal_MultiplyScalar(ref vector1, scaleFactor);
            return vector1;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_MultiplyTwoVectors", CallingConvention = CallingConvention.StdCall)]
        private static extern float Internal_MultiplyTwoVectors(ref Vector3 vector, ref Vector3 vector2);

        public static Vector3 operator *(Vector3 vector1, Vector3 vector2)
        {
            Internal_MultiplyTwoVectors(ref vector1, ref vector2);
            return vector1;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Vector3_MultiplyDivision", CallingConvention = CallingConvention.StdCall)]
        private static extern float Internal_MultiplyDivision(ref Vector3 vector, float scalar);

        public static Vector3 operator /(Vector3 vector, float divider)
        {
            Internal_MultiplyDivision(ref vector, divider);
            return vector;
        }

        public static bool operator ==(Vector3 value1, Vector3 value2)
        {
            return value1.X == value2.X && value1.Y == value2.Y && value1.Z == value2.Z;
        }

        public static bool operator !=(Vector3 value1, Vector3 value2)
        {
            return value1.X != value2.X || value1.Y != value2.Y || value1.Z != value2.Z;
        }

		public bool Equals(Vector3 other)
        {
            return this.X == other.X && this.Y == other.Y && this.Z == other.Z;
        }

        public override bool Equals(object obj)
        {
            bool result = false;
            if (obj is Vector3)
            {
                result = this.Equals((Vector3)obj);
            }
            return result;
        }

        public override int GetHashCode()
        {
            return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode();
        }

        public override string ToString()
        {
            return $"{{X = {X}; Y = {Y}; Z = {Z}}}";
        }
    }
}
