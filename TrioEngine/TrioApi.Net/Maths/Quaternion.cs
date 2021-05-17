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

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Quaternion_CreateFromYawPitchRoll", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_CreateFromYawPitchRoll(ref Quaternion quaternion, float yaw, float pitch, float roll);

        public static Quaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll)
        {
            Quaternion result = Quaternion.Identity;
            Internal_CreateFromYawPitchRoll(ref result, yaw, pitch, roll);
            return result;
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
    }
}
