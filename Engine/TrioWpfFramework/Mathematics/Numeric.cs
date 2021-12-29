using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Mathematics
{
    public static class Numeric
    {
        [StructLayout(LayoutKind.Explicit)]
        private struct SingleToUInt32
        {
            [FieldOffset(0)]
            internal float Single;
            [FieldOffset(0)]
            internal UInt32 UInt32;
        }

        [StructLayout(LayoutKind.Explicit)]
        private struct DoubleToUInt64
        {
            [FieldOffset(0)]
            internal double Double;
            [FieldOffset(0)]
            internal UInt64 UInt64;
        }


        private static float _epsilonF = 1e-5f;
        private static float _epsilonFSquared = 1e-5f * 1e-5f;
        private static double _epsilonD = 1e-12;
        private static double _epsilonDSquared = 1e-12 * 1e-12;

        public static float EpsilonF
        {
            get { return _epsilonF; }
            set
            {
                if (value <= 0)
                    throw new ArgumentOutOfRangeException("value", "The tolerance value must be greater than 0.");

                _epsilonF = value;
                _epsilonFSquared = value * value;
            }
        }

        public static float EpsilonFSquared
        {
            get { return _epsilonFSquared; }
        }

        public static bool IsNaN(float value)
        {
            // IEEE 754: 
            //   msb means most significant bit
            //   lsb means least significant bit
            //    1    8              23             ... widths
            //   +-+-------+-----------------------+
            //   |s|  exp  |          man          |
            //   +-+-------+-----------------------+
            //      msb lsb msb                 lsb  ... order
            //  
            //  If exp = 255 and man != 0, then value is NaN regardless of s.
            //
            // => If the argument is any value in the range 0x7f800001 through 0x7fffffff or in the range 
            // 0xff800001 through 0xffffffff, the result will be NaN.
            SingleToUInt32 t = new SingleToUInt32 { Single = value };

            UInt32 exp = t.UInt32 & 0x7f800000;
            UInt32 man = t.UInt32 & 0x007fffff;

            return exp == 0x7f800000 && man != 0;
        }

        public static bool IsNaN(double value)
        {
            // IEEE 754: 
            //   msb means most significant bit
            //   lsb means least significant bit
            //    1   11              52             ... widths
            //   +-+-------+-----------------------+
            //   |s|  exp  |          man          |
            //   +-+-------+-----------------------+
            //      msb lsb msb                 lsb  ... order
            //  
            //  If exp = 2047 and man != 0, then value is NaN regardless of s.
            //
            // => If the argument is any value in the range 0x7ff0000000000001L through 
            // 0x7fffffffffffffffL or in the range 0xfff0000000000001L through 0xffffffffffffffffL, the 
            // result will be NaN.
            DoubleToUInt64 t = new DoubleToUInt64 { Double = value };

            UInt64 exp = t.UInt64 & 0x7ff0000000000000;
            UInt64 man = t.UInt64 & 0x000fffffffffffff;

            return exp == 0x7ff0000000000000 && man != 0;
        }

        /// <overloads>
        /// <summary>
        /// Determines whether the specified value is finite.
        /// </summary>
        /// </overloads>
        /// 
        /// <summary>
        /// Determines whether the specified value is finite.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>
        /// <see langword="true"/> if <paramref name="value"/> is finite; otherwise, 
        /// <see langword="false"/>.
        /// </returns>
        public static bool IsFinite(float value)
        {
            return !IsNaN(value) && !Single.IsInfinity(value);
        }


        /// <summary>
        /// Determines whether the specified value is finite.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>
        /// <see langword="true"/> if <paramref name="value"/> is finite; otherwise, 
        /// <see langword="false"/>.
        /// </returns>
        public static bool IsFinite(double value)
        {
            return !IsNaN(value) && !Double.IsInfinity(value);
        }

        public static bool IsLessOrEqual(double value1, double value2)
        {
            return (value1 < value2) || AreEqual(value1, value2);
        }

        public static bool AreEqual(double value1, double value2)
        {
            // Infinity values have to be handled carefully because the check with the epsilon tolerance
            // does not work there. Check for equality in case they are infinite:
            if (value1 == value2)
                return true;

            // Scale epsilon proportional the given values.
            double epsilon = _epsilonD * (Math.Abs(value1) + Math.Abs(value2) + 1.0);
            double delta = value1 - value2;
            return (-epsilon < delta) && (delta < epsilon);

            // We could also use ... Abs(v1 - v2) <= _epsilonF * Max(Abs(v1), Abs(v2), 1)
        }

        public static bool AreEqual(float value1, float value2, float epsilon)
        {
            if (epsilon <= 0.0f)
                throw new ArgumentOutOfRangeException("epsilon", "Epsilon value must be greater than 0.");

            // Infinity values have to be handled carefully because the check with the epsilon tolerance
            // does not work there. Check for equality in case they are infinite:
            if (value1 == value2)
                return true;

            float delta = value1 - value2;
            return (-epsilon < delta) && (delta < epsilon);
        }

        public static bool AreEqual(double value1, double value2, double epsilon)
        {
            if (epsilon <= 0.0)
                throw new ArgumentOutOfRangeException("epsilon", "Epsilon value must be greater than 0.");

            // Infinity values have to be handled carefully because the check with the epsilon tolerance
            // does not work there. Check for equality in case they are infinite:
            if (value1 == value2)
                return true;

            double delta = value1 - value2;
            return (-epsilon < delta) && (delta < epsilon);
        }


        public static bool IsLess(double value1, double value2)
        {
            return (value1 < value2) && !AreEqual(value1, value2);
        }

        public static bool IsLess(double value1, double value2, double epsilon)
        {
            return (value1 < value2) && !AreEqual(value1, value2, epsilon);
        }

        public static bool IsLessOrEqual(float value1, float value2)
        {
            return (value1 < value2) || AreEqual(value1, value2);
        }

        public static bool IsLessOrEqual(float value1, float value2, float epsilon)
        {
            return (value1 < value2) || AreEqual(value1, value2, epsilon);
        }

        public static bool IsLessOrEqual(double value1, double value2, double epsilon)
        {
            return (value1 < value2) || AreEqual(value1, value2, epsilon);
        }

        public static bool IsGreater(float value1, float value2)
        {
            return (value1 > value2) && !AreEqual(value1, value2);
        }

        public static bool IsGreater(float value1, float value2, float epsilon)
        {
            return (value1 > value2) && !AreEqual(value1, value2, epsilon);
        }
        public static bool IsGreater(double value1, double value2)
        {
            return (value1 > value2) && !AreEqual(value1, value2);
        }

        public static bool IsGreater(double value1, double value2, double epsilon)
        {
            return (value1 > value2) && !AreEqual(value1, value2, epsilon);
        }

        public static bool IsGreaterOrEqual(float value1, float value2)
        {
            return (value1 > value2) || AreEqual(value1, value2);
        }

        public static bool IsGreaterOrEqual(float value1, float value2, float epsilon)
        {
            return (value1 > value2) || AreEqual(value1, value2, epsilon);
        }

        public static bool IsGreaterOrEqual(double value1, double value2)
        {
            return (value1 > value2) || AreEqual(value1, value2);
        }

        public static bool IsZero(float value)
        {
            return (-_epsilonF < value) && (value < _epsilonF);
        }

        public static bool IsZero(double value)
        {
            return (-_epsilonD < value) && (value < _epsilonD);
        }

        public static bool IsZero(double value, double epsilon)
        {
            if (epsilon <= 0.0)
                throw new ArgumentOutOfRangeException("epsilon", "Epsilon value must be greater than 0.");

            return (-epsilon < value) && (value < epsilon);
        }
    }
}
