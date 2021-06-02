
namespace TrioApi.Net.Maths
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
    }
}
