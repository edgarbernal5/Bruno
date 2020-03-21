using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Helpers
{
    public static class ObjectHelper
    {
        public static T ConvertTo<T>(object value)
        {
            // TODO: For more advanced conversions take a look at Json.NET (Newtonsoft.Json\Src\Newtonsoft.Json\Utilities\ConvertUtils.cs).

            if (value is T)
                return (T)value;

            return (T)Convert.ChangeType(value, typeof(T));
        }

        public static T ConvertTo<T>(object value, IFormatProvider provider)
        {
            // TODO: For more advanced conversions take a look at Json.NET (Newtonsoft.Json\Src\Newtonsoft.Json\Utilities\ConvertUtils.cs).

            if (value is T)
                return (T)value;

            return (T)Convert.ChangeType(value, typeof(T), provider);
        }
    }
}
