
using System;

namespace BrunoApi.Net.Maths
{
    public static class MathHelper
    {
        public const float PI = 3.14159274f;
        public const float TwoPi = 6.28318548f;

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
    }
}
