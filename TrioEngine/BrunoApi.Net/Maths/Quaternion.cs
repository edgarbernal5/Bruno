using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace BrunoApi.Net.Maths
{
    public struct Quaternion : IEquatable<Quaternion>
    {

        public float X;
        public float Y;
        public float Z;
        public float W;

        private static Quaternion _identity = new Quaternion(0f, 0f, 0f, 1f);

        /// <summary>Returns a Quaternion representing no rotation.</summary>
        public static Quaternion Identity
        {
            get
            {
                return Quaternion._identity;
            }
        }

        public Quaternion(float x, float y, float z, float w)
        {
            this.X = x;
            this.Y = y;
            this.Z = z;
            this.W = w;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Quaternion_CreateFromAxisAngle", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_CreateFromAxisAngle(ref Quaternion quaternion, ref Vector3 axis, float angle);

        public static Quaternion CreateFromAxisAngle(Vector3 axis, float angle)
        {
            Quaternion result = Quaternion.Identity;
            Internal_CreateFromAxisAngle(ref result, ref axis, angle);
            return result;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Quaternion_CreateFromMatrix", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_CreateFromMatrix(ref Quaternion quaternion, ref Matrix matrix);

        public static Quaternion CreateFromMatrix(Matrix matrix)
        {
            Quaternion result = Quaternion.Identity;
            Internal_CreateFromMatrix(ref result, ref matrix);
            return result;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Quaternion_CreateFromYawPitchRoll", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_CreateFromYawPitchRoll(ref Quaternion quaternion, float yaw, float pitch, float roll);

        public static Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll)
        {
            Quaternion result = Quaternion.Identity;
            Internal_CreateFromYawPitchRoll(ref result, yaw, pitch, roll);
            return result;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Quaternion_Inverse", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Inverse(ref Quaternion quaternion);

        public static Quaternion Inverse(Quaternion quaternion)
        {
            Internal_Inverse(ref quaternion);
            return quaternion;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Quaternion_MultiplyTwoQuats", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_MultiplyTwoQuats(ref Quaternion quaternion1, ref Quaternion quaternion2);

        public static Quaternion operator *(Quaternion quaternion1, Quaternion quaternion2)
        {
            Internal_MultiplyTwoQuats(ref quaternion1, ref quaternion2);
            return quaternion1;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Quaternion_Normalize", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Normalize(ref Quaternion quaternion);

        public void Normalize()
        {
            Internal_Normalize(ref this);
        }

        public static Quaternion CreateFromYawPitchRoll(Vector3 pitchYawRoll)
        {
            Quaternion result = Quaternion.Identity;
            Internal_CreateFromYawPitchRoll(ref result, pitchYawRoll.Y, pitchYawRoll.X, pitchYawRoll.Z);
            return result;
        }

        public static Vector3 EulerAngles(Quaternion quaternion)
        {
            var angles = new Vector3();

            var matrix = Matrix.CreateFromQuaternion(quaternion);

            angles.X = (float)Math.Atan2(matrix.M23, matrix.M33);
            angles.Y = (float)Math.Atan2(-matrix.M13, Math.Sqrt(matrix.M23 * matrix.M23 + matrix.M33 * matrix.M33));
            angles.Z = (float)Math.Atan2(matrix.M12, matrix.M11);

            /*
            ***ImGuizmo.cpp
            void DecomposeMatrixToComponents(const float* matrix, float* translation, float* rotation, float* scale)
            rotation[0] = RAD2DEG * atan2f(mat.m[1][2], mat.m[2][2]);
            rotation[1] = RAD2DEG * atan2f(-mat.m[0][2], sqrtf(mat.m[1][2] * mat.m[1][2] + mat.m[2][2] * mat.m[2][2]));
            rotation[2] = RAD2DEG * atan2f(mat.m[0][1], mat.m[0][0]);
            */

            //angles.Y = (float)Math.Atan2(2.0f * (quaternion.Y * quaternion.W + quaternion.X * quaternion.Z), 1.0f - 2.0f * (quaternion.X * quaternion.X + quaternion.Y * quaternion.Y));
            //angles.X = (float)Math.Asin(2.0f * (quaternion.X * quaternion.W - quaternion.Y * quaternion.Z));
            //angles.Z = (float)Math.Atan2(2.0f * (quaternion.X * quaternion.Y + quaternion.Z * quaternion.W), 1.0f - 2.0f * (quaternion.X * quaternion.X + quaternion.Z * quaternion.Z));

            return angles;
        }

        public bool Equals(Quaternion other)
        {
            return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
        }

        public override bool Equals(object obj)
        {
            bool result = false;
            if (obj is Quaternion)
            {
                result = Equals((Quaternion)obj);
            }
            return result;
        }

        public static bool operator ==(Quaternion value1, Quaternion value2)
        {
            return value1.X == value2.X && value1.Y == value2.Y && value1.Z == value2.Z && value1.W == value2.W;
        }

        public static bool operator !=(Quaternion value1, Quaternion value2)
        {
            return value1.X != value2.X || value1.Y != value2.Y || value1.Z != value2.Z || value1.W != value2.W;
        }

        public override int GetHashCode()
        {
            return X.GetHashCode() + Y.GetHashCode() + Z.GetHashCode() + W.GetHashCode();
        }

        public override string ToString()
        {
            return string.Format("{{X:{0} Y:{1} Z:{2} W:{3}}}", 
                X.ToString(),
                Y.ToString(),
                Z.ToString(),
                W.ToString()
            );
        }

        /*
         EulerAngles ToEulerAngles(Quaternion q) {
    EulerAngles angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.roll = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = 2 * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1)
        angles.pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        angles.pitch = std::asin(sinp);

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.yaw = std::atan2(siny_cosp, cosy_cosp);

    return angles;
}
         */
    }
}
