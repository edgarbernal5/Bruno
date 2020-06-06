
using System;
using System.Windows;
using System.Windows.Media;

namespace EsteroWindows
{
    public static class Boxed
    {
        public static readonly object BooleanFalse = false;

        public static readonly object BooleanTrue = true;

        public static readonly object Int32Zero = 0;

        public static readonly object Int32One = 1;

        public static readonly object SingleZero = 0.0f;

        public static readonly object SingleOne = 1.0f;

        public static readonly object SinglePositiveInfinity = float.PositiveInfinity;

        public static readonly object SingleNegativeInfinity = float.NegativeInfinity;

        public static readonly object SingleNaN = float.NaN;

        public static readonly object DoubleZero = 0.0d;

        public static readonly object DoubleOne = 1.0d;

        public static readonly object DoublePositiveInfinity = double.PositiveInfinity;

        public static readonly object DoubleNegativeInfinity = double.NegativeInfinity;

        public static readonly object DoubleNaN = double.NaN;

        public static readonly object ColorWhite = Colors.White;

        public static readonly object ColorBlack = Colors.Black;

        public static readonly object PointZero = new Point(0, 0);

        public static readonly object PointNaN = new Point(double.NaN, double.NaN);

        public static readonly object TimeSpanZero = new TimeSpan();


        public static object Get(bool value)
        {
            return value ? BooleanTrue : BooleanFalse;
        }
    }
}
