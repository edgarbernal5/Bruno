
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Maths
{
    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct Matrix : IEquatable<Matrix>
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_CreateLookAt", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_CreateLookAt(ref Matrix mat, ref Vector3 eye, ref Vector3 target, ref Vector3 up);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_CreatePerspectiveFieldOfView", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_CreatePerspectiveFieldOfView(ref Matrix mat, float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_CreateRotationX", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_CreateRotationX(ref Matrix mat, float radians);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_CreateRotationY", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_CreateRotationY(ref Matrix mat, float radians);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_CreateTranslation", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_CreateTranslation(ref Matrix mat, ref Vector3 position);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_Division", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Division(ref Matrix mat, ref Matrix mat2);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_Multiply", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Multiply(ref Matrix mat, ref Matrix mat2);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_MultiplyScalar", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_MultiplyScalar(ref Matrix mat, float scalar);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_Sub", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Sub(ref Matrix mat, ref Matrix mat2);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_Sum", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Sum(ref Matrix mat, ref Matrix mat2);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_Transpose", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Transpose(ref Matrix mat);

        public float M11;
        public float M12;
        public float M13;
        public float M14;
        public float M21;
        public float M22;
        public float M23;
        public float M24;
        public float M31;
        public float M32;
        public float M33;
        public float M34;
        public float M41;
        public float M42;
        public float M43;
        public float M44;

        private static Matrix m_identity, m_zero;

        public static Matrix Identity
        {
            get
            {
                return m_identity;
            }
        }

        public static Matrix Zero
        {
            get
            {
                return m_zero;
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_GetRight", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetRight(ref Matrix matrix, ref Vector3 result);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_SetRight", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetRight(ref Matrix matrix, ref Vector3 result);

        public Vector3 Right
        {
            get
            {
                Vector3 result = new Vector3();
                Internal_GetRight(ref this, ref result);
                return result;
            }
            set
            {
                Internal_SetRight(ref this, ref value);
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_GetUp", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetUp(ref Matrix matrix, ref Vector3 result);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_SetUp", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetUp(ref Matrix matrix, ref Vector3 result);

        public Vector3 Up
        {
            get
            {
                Vector3 result = new Vector3();
                Internal_GetUp(ref this, ref result);
                return result;
            }
            set
            {
                Internal_SetUp(ref this, ref value);
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_GetForward", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetForward(ref Matrix matrix, ref Vector3 result);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_SetForward", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetForward(ref Matrix matrix, ref Vector3 result);

        public Vector3 Forward
        {
            get
            {
                Vector3 result = new Vector3();
                Internal_GetForward(ref this, ref result);
                return result;
            }
            set
            {
                Internal_SetForward(ref this, ref value);
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_GetTranslation", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetTranslation(ref Matrix matrix, ref Vector3 result);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_SetTranslation", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetTranslation(ref Matrix matrix, ref Vector3 result);

        public Vector3 Translation
        {
            get
            {
                Vector3 result = new Vector3();
                Internal_GetTranslation(ref this, ref result);
                return result;
            }
            set
            {
                Internal_SetTranslation(ref this, ref value);
            }
        }

        public Matrix(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
        {
            M11 = m11;
            M12 = m12;
            M13 = m13;
            M14 = m14;
            M21 = m21;
            M22 = m22;
            M23 = m23;
            M24 = m24;
            M31 = m31;
            M32 = m32;
            M33 = m33;
            M34 = m34;
            M41 = m41;
            M42 = m42;
            M43 = m43;
            M44 = m44;
        }

        public static object CreateCreateFromAxisAngle(object right, float x)
        {
            throw new NotImplementedException();
        }

        public unsafe Matrix(float* val)
        {
            this.M11 = val[0];
            this.M12 = val[1];
            this.M13 = val[2];
            this.M14 = val[3];
            this.M21 = val[4];
            this.M22 = val[5];
            this.M23 = val[6];
            this.M24 = val[7];
            this.M31 = val[8];
            this.M32 = val[9];
            this.M33 = val[10];
            this.M34 = val[11];
            this.M41 = val[12];
            this.M42 = val[13];
            this.M43 = val[14];
            this.M44 = val[15];
        }

        static Matrix()
        {
            m_identity = new Matrix(1f, 0f, 0f, 0f, 0f, 1f, 0f, 0f, 0f, 0f, 1f, 0f, 0f, 0f, 0f, 1f);
            m_zero = new Matrix();
        }

        public float[] ToArray()
        {
            float[] array = new float[16];
            array[0] = M11;
            array[1] = M12;
            array[2] = M13;
            array[3] = M14;

            array[4] = M21;
            array[5] = M22;
            array[6] = M23;
            array[7] = M24;

            array[8] = M31;
            array[9] = M32;
            array[10] = M33;
            array[11] = M34;

            array[12] = M41;
            array[13] = M42;
            array[14] = M43;
            array[15] = M44;
            return array;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_CreateFromAxisAngle", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_CreateFromAxisAngle(ref Matrix matrix, ref Vector3 axis, float angle);
        //
        public static Matrix CreateCreateFromAxisAngle(Vector3 axis, float angle)
        {
            Matrix result = Matrix.Identity;
            Internal_CreateFromAxisAngle(ref result, ref axis, angle);
            return result;
        }

        public static Matrix CreateLookAt(Vector3 eye, Vector3 target, Vector3 up)
        {
            var matrix = new Matrix();
            Internal_CreateLookAt(ref matrix, ref eye, ref target, ref up);

            return matrix;
        }

        public static Matrix CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
        {
            var matrix = new Matrix();
            Internal_CreatePerspectiveFieldOfView(ref matrix, fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance);

            return matrix;
        }

        public static Matrix CreateRotationX(float radians)
        {
            var matrix = new Matrix();
            Internal_CreateRotationX(ref matrix, radians);

            return matrix;
        }

        public static Matrix CreateRotationY(float radians)
        {
            var matrix = new Matrix();
            Internal_CreateRotationY(ref matrix, radians);

            return matrix;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_CreateRotationZ", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_CreateRotationZ(ref Matrix mat, float radians);

        public static Matrix CreateRotationZ(float radians)
        {
            var matrix = new Matrix();
            Internal_CreateRotationZ(ref matrix, radians);

            return matrix;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_CreateScale", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_CreateScale(ref Matrix mat, ref Vector3 scales);

        public static Matrix CreateScale(Vector3 scales)
        {
            var matrix = new Matrix();
            Internal_CreateScale(ref matrix, ref scales);

            return matrix;
        }

        public static Matrix CreateTranslation(Vector3 position)
        {
            var matrix = new Matrix();
            Internal_CreateTranslation(ref matrix, ref position);

            return matrix;
        }

        public static Matrix operator /(Matrix matrix1, Matrix matrix2)
        {
            Internal_Division(ref matrix1, ref matrix2);
            return matrix1;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_Invert", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Invert(ref Matrix mat);

        public static Matrix Invert(Matrix matrix)
        {
            Internal_Invert(ref matrix);

            return matrix;
        }

        public static Matrix operator *(Matrix matrix1, Matrix matrix2)
        {
            Internal_Multiply(ref matrix1, ref matrix2);
            return matrix1;
        }

        public static Matrix operator *(Matrix matrix, float scaleFactor)
        {
            Internal_MultiplyScalar(ref matrix, scaleFactor);
            return matrix;
        }

        public static Matrix operator *(float scaleFactor, Matrix matrix)
        {
            Internal_MultiplyScalar(ref matrix, scaleFactor);
            return matrix;
        }

        public static Matrix operator -(Matrix matrix1, Matrix matrix2)
        {
            Internal_Sub(ref matrix1, ref matrix2);
            return matrix1;
        }

        public static Matrix operator +(Matrix matrix1, Matrix matrix2)
        {
            Internal_Sum(ref matrix1, ref matrix2);
            return matrix1;
        }

        public static Matrix Transpose(Matrix matrix)
        {
            Internal_Transpose(ref matrix);
            return matrix;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_CreateTRS", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_CreateTRS(ref Matrix mat, ref Vector3 position, ref Quaternion rotation, ref Vector3 scale);

        public static Matrix CreateTRS(Vector3 position, Quaternion rotation, Vector3 scale)
        {
            var matrix = new Matrix();
            Internal_CreateTRS(ref matrix, ref position, ref rotation, ref scale);
            return matrix;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Matrix_CreateWorld", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_CreateWorld(ref Matrix mat, ref Vector3 position, ref Vector3 forward, ref Vector3 up);

        public static Matrix CreateWorld(Vector3 position, Vector3 forward, Vector3 up)
        {
            var matrix = new Matrix();
            Internal_CreateWorld(ref matrix, ref position, ref forward, ref up);
            return matrix;
        }

        public static Vector3 CreateEulerAnglesFromRotation(Matrix matrix)
        {
            Vector3 eulerAngles = new Vector3();

            eulerAngles.Y = (float)Math.Asin(-matrix.M32);                  // Pitch
            if (Math.Cos(eulerAngles.Y) > 0.0001)                 // Not at poles
            {
                eulerAngles.X = (float)Math.Atan2(matrix.M31, matrix.M33);     // Yaw
                eulerAngles.Z = (float)Math.Atan2(matrix.M12, matrix.M22);     // Roll
            }
            else
            {
                eulerAngles.X = 0.0f;                         // Yaw
                eulerAngles.Z = (float)Math.Atan2(-matrix.M21, matrix.M11);    // Roll
            }
            return eulerAngles;
        }

        /// <summary>Determines whether the specified Object is equal to the Matrix.</summary>
        /// <param name="other">The Object to compare with the current Matrix.</param>
        public bool Equals(Matrix other)
        {
            return this.M11 == other.M11 && this.M22 == other.M22 && this.M33 == other.M33 && this.M44 == other.M44 && this.M12 == other.M12 && this.M13 == other.M13 && this.M14 == other.M14 && this.M21 == other.M21 && this.M23 == other.M23 && this.M24 == other.M24 && this.M31 == other.M31 && this.M32 == other.M32 && this.M34 == other.M34 && this.M41 == other.M41 && this.M42 == other.M42 && this.M43 == other.M43;
        }

        /// <summary>Returns a value that indicates whether the current instance is equal to a specified object.</summary>
        /// <param name="obj">Object with which to make the comparison.</param>
        public override bool Equals(object obj)
        {
            bool result = false;
            if (obj is Matrix)
            {
                result = this.Equals((Matrix)obj);
            }
            return result;
        }

        /// <summary>Gets the hash code of this object.</summary>
        public override int GetHashCode()
        {
            return this.M11.GetHashCode() + this.M12.GetHashCode() + this.M13.GetHashCode() + this.M14.GetHashCode() + this.M21.GetHashCode() + this.M22.GetHashCode() + this.M23.GetHashCode() + this.M24.GetHashCode() + this.M31.GetHashCode() + this.M32.GetHashCode() + this.M33.GetHashCode() + this.M34.GetHashCode() + this.M41.GetHashCode() + this.M42.GetHashCode() + this.M43.GetHashCode() + this.M44.GetHashCode();
        }
    }
}
