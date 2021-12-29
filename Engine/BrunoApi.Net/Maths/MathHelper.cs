
using System;

namespace BrunoApi.Net.Maths
{
    public static class MathHelper
    {
        public const float PI = 3.14159274f;
        public const float TwoPi = 6.28318548f; 
        public const float TwoPiInDegrees = 360.0f;

        public static float ToRadians(float degrees)
        {
            return degrees * 0.0174532924f;
        }

        public static float ToDegrees(float radians)
        {
            return radians * 57.2957764f;
        }

        public static float Clamp(float v, float min, float max)
        {
            if (v < min)
                return min;
            if (v > max)
                return max;

            return v;
        }

        public static float Acos(float v)
        {
            return (float)Math.Acos(v);
        }

        public static Vector3 ToRadians(Vector3 degrees)
        {
            return new Vector3(ToRadians(degrees.X), ToRadians(degrees.Y), ToRadians(degrees.Z));
        }

        public static Vector3 ToDegrees(Vector3 radians)
        {
            return new Vector3(ToDegrees(radians.X), ToDegrees(radians.Y), ToDegrees(radians.Z));
        }

        public static Vector3 NormalizeAngles(Vector3 degrees)
        {
            while (degrees.X <= -TwoPiInDegrees)
            {
                degrees.X += TwoPiInDegrees;
            }
            while (degrees.X >= TwoPiInDegrees)
            {
                degrees.X -= TwoPiInDegrees;
            }

            while (degrees.Y <= -TwoPiInDegrees)
            {
                degrees.Y += TwoPiInDegrees;
            }
            while (degrees.Y >= TwoPiInDegrees)
            {
                degrees.Y -= TwoPiInDegrees;
            }

            while (degrees.Z <= -TwoPiInDegrees)
            {
                degrees.Z += TwoPiInDegrees;
            }
            while (degrees.Z >= TwoPiInDegrees)
            {
                degrees.Z -= TwoPiInDegrees;
            }

            return degrees;
        }
    }
}
