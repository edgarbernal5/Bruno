
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace BrunoApi.Net.Graphics.Core
{
    [DebuggerDisplay("R:{R} G:{G} B:{B} A:{A}")]
    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    public struct Color : IEquatable<Color>
    {
        public float R;
        public float G;
        public float B;
        public float A;

        public Color(float sameValue)
        {
            R = G = B = A = sameValue;
        }

        public Color(float r, float g, float b)
        {
            R = r;
            G = g;
            B = b;
            A = 1.0f;
        }

        public Color(float r, float g, float b, float a)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }

		public override int GetHashCode()
        {
            return R.GetHashCode() + G.GetHashCode() + B.GetHashCode() + A.GetHashCode();
        }

        public override bool Equals(object obj)
        {
            return obj is Color && this.Equals((Color)obj);
        }

        public bool Equals(Color other)
        {
            return R.Equals(other.R) && G.Equals(other.G) && B.Equals(other.B) && A.Equals(other.A);
        }

        public static bool operator ==(Color a, Color b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(Color a, Color b)
        {
            return !a.Equals(b);
        }

        #region Colors

        public static Color Transparent
        {
            get
            {
                return new Color(0);
            }
        }

        public static Color Red
        {
            get
            {
                return new Color(1.0f, 0.0f, 0.0f);
            }
        }

        public static Color CornflowerBlue
        {
            get
            {
                return new Color(1.0f, 0.0f, 0.0f);
            }
        }

        #endregion

    }
}
