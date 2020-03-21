namespace TrioWpfFramework.Windows
{
    public static class Boxed
    {
        public static readonly object Int32Zero = 0;
        public static readonly object BooleanFalse = false;
        public static readonly object BooleanTrue = true;

        /// <summary>
        /// The value 0 as <see cref="double"/>.
        /// </summary>
        public static readonly object DoubleZero = 0.0d;


        /// <summary>
        /// The value 1 as <see cref="double"/>.
        /// </summary>
        public static readonly object DoubleOne = 1.0d;

        public static object Get(bool value)
        {
            return value ? BooleanTrue : BooleanFalse;
        }


    }
}
