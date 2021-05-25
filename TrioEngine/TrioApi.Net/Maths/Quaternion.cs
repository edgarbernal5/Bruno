using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioApi.Net.Maths
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

        /// <summary>Determines whether the specified Object is equal to the Quaternion.</summary>
        /// <param name="other">The Quaternion to compare with the current Quaternion.</param>
        public bool Equals(Quaternion other)
        {
            return this.X == other.X && this.Y == other.Y && this.Z == other.Z && this.W == other.W;
        }

        /// <summary>Returns a value that indicates whether the current instance is equal to a specified object.</summary>
        /// <param name="obj">Object to make the comparison with.</param>
        public override bool Equals(object obj)
        {
            bool result = false;
            if (obj is Quaternion)
            {
                result = this.Equals((Quaternion)obj);
            }
            return result;
        }

        /// <summary>Get the hash code of this object.</summary>
        public override int GetHashCode()
        {
            return this.X.GetHashCode() + this.Y.GetHashCode() + this.Z.GetHashCode() + this.W.GetHashCode();
        }

        public static Vector3 EulerAngles(Quaternion q)
        {
            Vector3 angles = new Vector3();
            //WIP

            // roll (x-axis rotation)
            double sinr_cosp = 2.0 * (q.W * q.X + q.Y * q.Z);
            double cosr_cosp = 1.0 - 2.0 * (q.X * q.X + q.Y * q.Y);
            angles.Y = (float)Math.Atan2(sinr_cosp, cosr_cosp);

            // pitch (y-axis rotation)
            double sinp = 2 * (q.W * q.Y - q.Z * q.X);
            if (Math.Abs(sinp) >= 1)
                angles.X = (float)(Math.PI / 2) * Math.Sign(sinp); // use 90 degrees if out of range
            else
                angles.X = (float)Math.Asin(sinp);

            // yaw (z-axis rotation)
            double siny_cosp = 2.0 * (q.W * q.Z + q.X * q.Y);
            double cosy_cosp = 1.0 - 2.0 * (q.Y * q.Y + q.Z * q.Z);
            angles.Z = (float)Math.Atan2(siny_cosp, cosy_cosp);

            return angles;
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
