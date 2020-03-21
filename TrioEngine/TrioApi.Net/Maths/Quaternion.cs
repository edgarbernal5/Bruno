using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Net.Maths
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
