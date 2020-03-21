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
            [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
            [FieldOffset(0)]
            internal float Single;
            [FieldOffset(0)]
            internal UInt32 UInt32;
        }

        [StructLayout(LayoutKind.Explicit)]
        private struct DoubleToUInt64
        {
            [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
            [FieldOffset(0)]
            internal double Double;
            [FieldOffset(0)]
            internal UInt64 UInt64;
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
    }
}
