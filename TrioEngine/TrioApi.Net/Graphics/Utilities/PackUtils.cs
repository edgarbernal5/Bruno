
namespace TrioWpfFramework.Net.Graphics.Utilities
{
    internal static class PackUtils
    {
        public static uint PackUnsigned(float bitmask, float value)
        {
            return (uint)ClampAndRound(value, 0f, bitmask);
        }

        public static uint PackSigned(uint bitmask, float value)
        {
            float num = bitmask >> 1;
            float min = -num - 1f;
            return (uint)((int)ClampAndRound(value, min, num) & (int)bitmask);
        }

        public static uint PackUNorm(float bitmask, float value)
        {
            value *= bitmask;
            return (uint)ClampAndRound(value, 0f, bitmask);
        }

        public static float UnpackUNorm(uint bitmask, uint value)
        {
            value &= bitmask;
            return value / bitmask;
        }

        public static uint PackSNorm(uint bitmask, float value)
        {
            float num = bitmask >> 1;
            value *= num;
            return (uint)((int)ClampAndRound(value, -num, num) & (int)bitmask);
        }

        public static float UnpackSNorm(uint bitmask, uint value)
        {
            uint num = bitmask + 1u >> 1;
            if ((value & num) != 0u)
            {
                if ((value & bitmask) == num)
                {
                    return -1f;
                }
                value |= ~bitmask;
            }
            else
            {
                value &= bitmask;
            }
            float num2 = bitmask >> 1;
            return (float)value / num2;
        }

        private static double ClampAndRound(float value, float min, float max)
        {
            if (float.IsNaN(value))
            {
                return 0.0;
            }
            if (float.IsInfinity(value))
            {
                return (double)(float.IsNegativeInfinity(value) ? min : max);
            }
            if (value < min)
            {
                return (double)min;
            }
            if (value > max)
            {
                return (double)max;
            }
            return System.Math.Round(value);
        }
    }
}
