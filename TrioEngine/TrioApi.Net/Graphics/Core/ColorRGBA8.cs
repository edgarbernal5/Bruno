
using System;
using System.Runtime.InteropServices;
using TrioApi.Net.Graphics.Utilities;
using TrioApi.Net.Maths;

namespace TrioApi.Net.Graphics.Core
{
    [StructLayout(LayoutKind.Sequential)]
    public struct ColorRGBA8 : IEquatable<ColorRGBA8>
    {
        public uint PackedColor
        {
            get
            {
                return m_packedColor;
            }
        }
        private uint m_packedColor;

        public ColorRGBA8(uint packedValue)
        {
            m_packedColor = packedValue;
        }

        public ColorRGBA8(int r, int g, int b)
        {
            if ((((r | g) | b) & -256) != 0)
            {
                r = ClampToByte64((long)r);
                g = ClampToByte64((long)g);
                b = ClampToByte64((long)b);
            }
            g = g << 8;
            b = b << 0x10;
            m_packedColor = (uint)(((r | g) | b) | -16777216);
        }

        public ColorRGBA8(int r, int g, int b, int a)
        {
            if (((((r | g) | b) | a) & -256) != 0)
            {
                r = ClampToByte32(r);
                g = ClampToByte32(g);
                b = ClampToByte32(b);
                a = ClampToByte32(a);
            }
            g = g << 8;
            b = b << 0x10;
            a = a << 0x18;
            m_packedColor = (uint)(((r | g) | b) | a);
        }

        public ColorRGBA8(float r, float g, float b)
        {
            m_packedColor = PackHelper(r, g, b, 1f);
        }

        public ColorRGBA8(float r, float g, float b, float a)
        {
            m_packedColor = PackHelper(r, g, b, a);
        }

        public Vector3 ToVector3()
        {
            Vector3 vector;
            vector.X = PackUtils.UnpackUNorm(0xFF, m_packedColor);
            vector.Y = PackUtils.UnpackUNorm(0xFF, m_packedColor >> 8);
            vector.Z = PackUtils.UnpackUNorm(0xFF, m_packedColor >> 0x10);
            return vector;
        }

        private static uint PackHelper(float vectorX, float vectorY, float vectorZ, float vectorW)
        {
            uint num4 = PackUtils.PackUNorm(255f, vectorX);
            uint num3 = PackUtils.PackUNorm(255f, vectorY) << 8;
            uint num2 = PackUtils.PackUNorm(255f, vectorZ) << 0x10;
            uint num = PackUtils.PackUNorm(255f, vectorW) << 0x18;
            return (((num4 | num3) | num2) | num);
        }

        private static int ClampToByte32(int value)
        {
            if (value < 0)
            {
                return 0;
            }
            if (value > 0xff)
            {
                return 0xff;
            }
            return value;
        }

        private static int ClampToByte64(long value)
        {
            if (value < 0L)
            {
                return 0;
            }
            if (value > 0xffL)
            {
                return 0xff;
            }
            return (int)value;
        }

        public byte R
        {
            get
            {
                return (byte)m_packedColor;
            }
            set
            {
                m_packedColor = (m_packedColor & 0xffffff00) | value;
            }
        }
        public byte G
        {
            get
            {
                return (byte)(m_packedColor >> 8);
            }
            set
            {
                this.m_packedColor = (m_packedColor & 0xffff00ff) | ((uint)(value << 8));
            }
        }
        public byte B
        {
            get
            {
                return (byte)(m_packedColor >> 0x10);
            }
            set
            {
                m_packedColor = (m_packedColor & 0xff00ffff) | ((uint)(value << 0x10));
            }
        }
        public byte A
        {
            get
            {
                return (byte)(m_packedColor >> 0x18);
            }
            set
            {
                m_packedColor = (m_packedColor & 0xffffff) | ((uint)(value << 0x18));
            }
        }

		public override int GetHashCode()
        {
            return m_packedColor.GetHashCode();
        }

        public override bool Equals(object obj)
        {
            return obj is ColorRGBA8 && this.Equals((ColorRGBA8)obj);
        }

        public bool Equals(ColorRGBA8 other)
        {
            return m_packedColor.Equals(other.m_packedColor);
        }

        public static bool operator ==(ColorRGBA8 a, ColorRGBA8 b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(ColorRGBA8 a, ColorRGBA8 b)
        {
            return !a.Equals(b);
        }

        #region Colors

        public static ColorRGBA8 Transparent
        {
            get
            {
                return new ColorRGBA8(0);
            }
        }
        public static ColorRGBA8 AliceBlue
        {
            get
            {
                return new ColorRGBA8(0xfffff8f0);
            }
        }
        public static ColorRGBA8 AntiqueWhite
        {
            get
            {
                return new ColorRGBA8(0xffd7ebfa);
            }
        }
        public static ColorRGBA8 Aqua
        {
            get
            {
                return new ColorRGBA8(0xffffff00);
            }
        }
        public static ColorRGBA8 Aquamarine
        {
            get
            {
                return new ColorRGBA8(0xffd4ff7f);
            }
        }
        public static ColorRGBA8 Azure
        {
            get
            {
                return new ColorRGBA8(0xfffffff0);
            }
        }
        public static ColorRGBA8 Beige
        {
            get
            {
                return new ColorRGBA8(0xffdcf5f5);
            }
        }
        public static ColorRGBA8 Bisque
        {
            get
            {
                return new ColorRGBA8(0xffc4e4ff);
            }
        }
        public static ColorRGBA8 Black
        {
            get
            {
                return new ColorRGBA8(0xff000000);
            }
        }
        public static ColorRGBA8 BlanchedAlmond
        {
            get
            {
                return new ColorRGBA8(0xffcdebff);
            }
        }
        public static ColorRGBA8 Blue
        {
            get
            {
                return new ColorRGBA8(0xffff0000);
            }
        }
        public static ColorRGBA8 BlueViolet
        {
            get
            {
                return new ColorRGBA8(0xffe22b8a);
            }
        }
        public static ColorRGBA8 Brown
        {
            get
            {
                return new ColorRGBA8(0xff2a2aa5);
            }
        }
        public static ColorRGBA8 BurlyWood
        {
            get
            {
                return new ColorRGBA8(0xff87b8de);
            }
        }
        public static ColorRGBA8 CadetBlue
        {
            get
            {
                return new ColorRGBA8(0xffa09e5f);
            }
        }
        public static ColorRGBA8 Chartreuse
        {
            get
            {
                return new ColorRGBA8(0xff00ff7f);
            }
        }
        public static ColorRGBA8 Chocolate
        {
            get
            {
                return new ColorRGBA8(0xff1e69d2);
            }
        }
        public static ColorRGBA8 Coral
        {
            get
            {
                return new ColorRGBA8(0xff507fff);
            }
        }
        public static ColorRGBA8 CornflowerBlue
        {
            get
            {
                return new ColorRGBA8(0xffed9564);
            }
        }
        public static ColorRGBA8 Cornsilk
        {
            get
            {
                return new ColorRGBA8(0xffdcf8ff);
            }
        }
        public static ColorRGBA8 Crimson
        {
            get
            {
                return new ColorRGBA8(0xff3c14dc);
            }
        }
        public static ColorRGBA8 Cyan
        {
            get
            {
                return new ColorRGBA8(0xffffff00);
            }
        }
        public static ColorRGBA8 DarkBlue
        {
            get
            {
                return new ColorRGBA8(0xff8b0000);
            }
        }
        public static ColorRGBA8 DarkCyan
        {
            get
            {
                return new ColorRGBA8(0xff8b8b00);
            }
        }
        public static ColorRGBA8 DarkGoldenrod
        {
            get
            {
                return new ColorRGBA8(0xff0b86b8);
            }
        }
        public static ColorRGBA8 DarkGray
        {
            get
            {
                return new ColorRGBA8(0xffa9a9a9);
            }
        }
        public static ColorRGBA8 DarkGreen
        {
            get
            {
                return new ColorRGBA8(0xff006400);
            }
        }
        public static ColorRGBA8 DarkKhaki
        {
            get
            {
                return new ColorRGBA8(0xff6bb7bd);
            }
        }
        public static ColorRGBA8 DarkMagenta
        {
            get
            {
                return new ColorRGBA8(0xff8b008b);
            }
        }
        public static ColorRGBA8 DarkOliveGreen
        {
            get
            {
                return new ColorRGBA8(0xff2f6b55);
            }
        }
        public static ColorRGBA8 DarkOrange
        {
            get
            {
                return new ColorRGBA8(0xff008cff);
            }
        }
        public static ColorRGBA8 DarkOrchid
        {
            get
            {
                return new ColorRGBA8(0xffcc3299);
            }
        }
        public static ColorRGBA8 DarkRed
        {
            get
            {
                return new ColorRGBA8(0xff00008b);
            }
        }
        public static ColorRGBA8 DarkSalmon
        {
            get
            {
                return new ColorRGBA8(0xff7a96e9);
            }
        }
        public static ColorRGBA8 DarkSeaGreen
        {
            get
            {
                return new ColorRGBA8(0xff8bbc8f);
            }
        }
        public static ColorRGBA8 DarkSlateBlue
        {
            get
            {
                return new ColorRGBA8(0xff8b3d48);
            }
        }
        public static ColorRGBA8 DarkSlateGray
        {
            get
            {
                return new ColorRGBA8(0xff4f4f2f);
            }
        }
        public static ColorRGBA8 DarkTurquoise
        {
            get
            {
                return new ColorRGBA8(0xffd1ce00);
            }
        }
        public static ColorRGBA8 DarkViolet
        {
            get
            {
                return new ColorRGBA8(0xffd30094);
            }
        }
        public static ColorRGBA8 DeepPink
        {
            get
            {
                return new ColorRGBA8(0xff9314ff);
            }
        }
        public static ColorRGBA8 DeepSkyBlue
        {
            get
            {
                return new ColorRGBA8(0xffffbf00);
            }
        }
        public static ColorRGBA8 DimGray
        {
            get
            {
                return new ColorRGBA8(0xff696969);
            }
        }
        public static ColorRGBA8 DodgerBlue
        {
            get
            {
                return new ColorRGBA8(0xffff901e);
            }
        }
        public static ColorRGBA8 Firebrick
        {
            get
            {
                return new ColorRGBA8(0xff2222b2);
            }
        }
        public static ColorRGBA8 FloralWhite
        {
            get
            {
                return new ColorRGBA8(0xfff0faff);
            }
        }
        public static ColorRGBA8 ForestGreen
        {
            get
            {
                return new ColorRGBA8(0xff228b22);
            }
        }
        public static ColorRGBA8 Fuchsia
        {
            get
            {
                return new ColorRGBA8(0xffff00ff);
            }
        }
        public static ColorRGBA8 Gainsboro
        {
            get
            {
                return new ColorRGBA8(0xffdcdcdc);
            }
        }
        public static ColorRGBA8 GhostWhite
        {
            get
            {
                return new ColorRGBA8(0xfffff8f8);
            }
        }
        public static ColorRGBA8 Gold
        {
            get
            {
                return new ColorRGBA8(0xff00d7ff);
            }
        }
        public static ColorRGBA8 Goldenrod
        {
            get
            {
                return new ColorRGBA8(0xff20a5da);
            }
        }
        public static ColorRGBA8 Gray
        {
            get
            {
                return new ColorRGBA8(0xff808080);
            }
        }
        public static ColorRGBA8 Green
        {
            get
            {
                return new ColorRGBA8(0xff008000);
            }
        }
        public static ColorRGBA8 GreenYellow
        {
            get
            {
                return new ColorRGBA8(0xff2fffad);
            }
        }
        public static ColorRGBA8 Honeydew
        {
            get
            {
                return new ColorRGBA8(0xfff0fff0);
            }
        }
        public static ColorRGBA8 HotPink
        {
            get
            {
                return new ColorRGBA8(0xffb469ff);
            }
        }
        public static ColorRGBA8 IndianRed
        {
            get
            {
                return new ColorRGBA8(0xff5c5ccd);
            }
        }
        public static ColorRGBA8 Indigo
        {
            get
            {
                return new ColorRGBA8(0xff82004b);
            }
        }
        public static ColorRGBA8 Ivory
        {
            get
            {
                return new ColorRGBA8(0xfff0ffff);
            }
        }
        public static ColorRGBA8 Khaki
        {
            get
            {
                return new ColorRGBA8(0xff8ce6f0);
            }
        }
        public static ColorRGBA8 Lavender
        {
            get
            {
                return new ColorRGBA8(0xfffae6e6);
            }
        }
        public static ColorRGBA8 LavenderBlush
        {
            get
            {
                return new ColorRGBA8(0xfff5f0ff);
            }
        }
        public static ColorRGBA8 LawnGreen
        {
            get
            {
                return new ColorRGBA8(0xff00fc7c);
            }
        }
        public static ColorRGBA8 LemonChiffon
        {
            get
            {
                return new ColorRGBA8(0xffcdfaff);
            }
        }
        public static ColorRGBA8 LightBlue
        {
            get
            {
                return new ColorRGBA8(0xffe6d8ad);
            }
        }
        public static ColorRGBA8 LightCoral
        {
            get
            {
                return new ColorRGBA8(0xff8080f0);
            }
        }
        public static ColorRGBA8 LightCyan
        {
            get
            {
                return new ColorRGBA8(0xffffffe0);
            }
        }
        public static ColorRGBA8 LightGoldenrodYellow
        {
            get
            {
                return new ColorRGBA8(0xffd2fafa);
            }
        }
        public static ColorRGBA8 LightGreen
        {
            get
            {
                return new ColorRGBA8(0xff90ee90);
            }
        }
        public static ColorRGBA8 LightGray
        {
            get
            {
                return new ColorRGBA8(0xffd3d3d3);
            }
        }
        public static ColorRGBA8 LightPink
        {
            get
            {
                return new ColorRGBA8(0xffc1b6ff);
            }
        }
        public static ColorRGBA8 LightSalmon
        {
            get
            {
                return new ColorRGBA8(0xff7aa0ff);
            }
        }
        public static ColorRGBA8 LightSeaGreen
        {
            get
            {
                return new ColorRGBA8(0xffaab220);
            }
        }
        public static ColorRGBA8 LightSkyBlue
        {
            get
            {
                return new ColorRGBA8(0xffface87);
            }
        }
        public static ColorRGBA8 LightSlateGray
        {
            get
            {
                return new ColorRGBA8(0xff998877);
            }
        }
        public static ColorRGBA8 LightSteelBlue
        {
            get
            {
                return new ColorRGBA8(0xffdec4b0);
            }
        }
        public static ColorRGBA8 LightYellow
        {
            get
            {
                return new ColorRGBA8(0xffe0ffff);
            }
        }
        public static ColorRGBA8 Lime
        {
            get
            {
                return new ColorRGBA8(0xff00ff00);
            }
        }
        public static ColorRGBA8 LimeGreen
        {
            get
            {
                return new ColorRGBA8(0xff32cd32);
            }
        }
        public static ColorRGBA8 Linen
        {
            get
            {
                return new ColorRGBA8(0xffe6f0fa);
            }
        }
        public static ColorRGBA8 Magenta
        {
            get
            {
                return new ColorRGBA8(0xffff00ff);
            }
        }
        public static ColorRGBA8 Maroon
        {
            get
            {
                return new ColorRGBA8(0xff000080);
            }
        }
        public static ColorRGBA8 MediumAquamarine
        {
            get
            {
                return new ColorRGBA8(0xffaacd66);
            }
        }
        public static ColorRGBA8 MediumBlue
        {
            get
            {
                return new ColorRGBA8(0xffcd0000);
            }
        }
        public static ColorRGBA8 MediumOrchid
        {
            get
            {
                return new ColorRGBA8(0xffd355ba);
            }
        }
        public static ColorRGBA8 MediumPurple
        {
            get
            {
                return new ColorRGBA8(0xffdb7093);
            }
        }
        public static ColorRGBA8 MediumSeaGreen
        {
            get
            {
                return new ColorRGBA8(0xff71b33c);
            }
        }
        public static ColorRGBA8 MediumSlateBlue
        {
            get
            {
                return new ColorRGBA8(0xffee687b);
            }
        }
        public static ColorRGBA8 MediumSpringGreen
        {
            get
            {
                return new ColorRGBA8(0xff9afa00);
            }
        }
        public static ColorRGBA8 MediumTurquoise
        {
            get
            {
                return new ColorRGBA8(0xffccd148);
            }
        }
        public static ColorRGBA8 MediumVioletRed
        {
            get
            {
                return new ColorRGBA8(0xff8515c7);
            }
        }
        public static ColorRGBA8 MidnightBlue
        {
            get
            {
                return new ColorRGBA8(0xff701919);
            }
        }
        public static ColorRGBA8 MintCream
        {
            get
            {
                return new ColorRGBA8(0xfffafff5);
            }
        }
        public static ColorRGBA8 MistyRose
        {
            get
            {
                return new ColorRGBA8(0xffe1e4ff);
            }
        }
        public static ColorRGBA8 Moccasin
        {
            get
            {
                return new ColorRGBA8(0xffb5e4ff);
            }
        }
        public static ColorRGBA8 NavajoWhite
        {
            get
            {
                return new ColorRGBA8(0xffaddeff);
            }
        }
        public static ColorRGBA8 Navy
        {
            get
            {
                return new ColorRGBA8(0xff800000);
            }
        }
        public static ColorRGBA8 OldLace
        {
            get
            {
                return new ColorRGBA8(0xffe6f5fd);
            }
        }
        public static ColorRGBA8 Olive
        {
            get
            {
                return new ColorRGBA8(0xff008080);
            }
        }
        public static ColorRGBA8 OliveDrab
        {
            get
            {
                return new ColorRGBA8(0xff238e6b);
            }
        }
        public static ColorRGBA8 Orange
        {
            get
            {
                return new ColorRGBA8(0xff00a5ff);
            }
        }
        public static ColorRGBA8 OrangeRed
        {
            get
            {
                return new ColorRGBA8(0xff0045ff);
            }
        }
        public static ColorRGBA8 Orchid
        {
            get
            {
                return new ColorRGBA8(0xffd670da);
            }
        }
        public static ColorRGBA8 PaleGoldenrod
        {
            get
            {
                return new ColorRGBA8(0xffaae8ee);
            }
        }
        public static ColorRGBA8 PaleGreen
        {
            get
            {
                return new ColorRGBA8(0xff98fb98);
            }
        }
        public static ColorRGBA8 PaleTurquoise
        {
            get
            {
                return new ColorRGBA8(0xffeeeeaf);
            }
        }
        public static ColorRGBA8 PaleVioletRed
        {
            get
            {
                return new ColorRGBA8(0xff9370db);
            }
        }
        public static ColorRGBA8 PapayaWhip
        {
            get
            {
                return new ColorRGBA8(0xffd5efff);
            }
        }
        public static ColorRGBA8 PeachPuff
        {
            get
            {
                return new ColorRGBA8(0xffb9daff);
            }
        }
        public static ColorRGBA8 Peru
        {
            get
            {
                return new ColorRGBA8(0xff3f85cd);
            }
        }
        public static ColorRGBA8 Pink
        {
            get
            {
                return new ColorRGBA8(0xffcbc0ff);
            }
        }
        public static ColorRGBA8 Plum
        {
            get
            {
                return new ColorRGBA8(0xffdda0dd);
            }
        }
        public static ColorRGBA8 PowderBlue
        {
            get
            {
                return new ColorRGBA8(0xffe6e0b0);
            }
        }
        public static ColorRGBA8 Purple
        {
            get
            {
                return new ColorRGBA8(0xff800080);
            }
        }
        public static ColorRGBA8 Red
        {
            get
            {
                return new ColorRGBA8(0xff0000ff);
            }
        }
        public static ColorRGBA8 RosyBrown
        {
            get
            {
                return new ColorRGBA8(0xff8f8fbc);
            }
        }
        public static ColorRGBA8 RoyalBlue
        {
            get
            {
                return new ColorRGBA8(0xffe16941);
            }
        }
        public static ColorRGBA8 SaddleBrown
        {
            get
            {
                return new ColorRGBA8(0xff13458b);
            }
        }
        public static ColorRGBA8 Salmon
        {
            get
            {
                return new ColorRGBA8(0xff7280fa);
            }
        }
        public static ColorRGBA8 SandyBrown
        {
            get
            {
                return new ColorRGBA8(0xff60a4f4);
            }
        }
        public static ColorRGBA8 SeaGreen
        {
            get
            {
                return new ColorRGBA8(0xff578b2e);
            }
        }
        public static ColorRGBA8 SeaShell
        {
            get
            {
                return new ColorRGBA8(0xffeef5ff);
            }
        }
        public static ColorRGBA8 Sienna
        {
            get
            {
                return new ColorRGBA8(0xff2d52a0);
            }
        }
        public static ColorRGBA8 Silver
        {
            get
            {
                return new ColorRGBA8(0xffc0c0c0);
            }
        }
        public static ColorRGBA8 SkyBlue
        {
            get
            {
                return new ColorRGBA8(0xffebce87);
            }
        }
        public static ColorRGBA8 SlateBlue
        {
            get
            {
                return new ColorRGBA8(0xffcd5a6a);
            }
        }
        public static ColorRGBA8 SlateGray
        {
            get
            {
                return new ColorRGBA8(0xff908070);
            }
        }
        public static ColorRGBA8 Snow
        {
            get
            {
                return new ColorRGBA8(0xfffafaff);
            }
        }
        public static ColorRGBA8 SpringGreen
        {
            get
            {
                return new ColorRGBA8(0xff7fff00);
            }
        }
        public static ColorRGBA8 SteelBlue
        {
            get
            {
                return new ColorRGBA8(0xffb48246);
            }
        }
        public static ColorRGBA8 Tan
        {
            get
            {
                return new ColorRGBA8(0xff8cb4d2);
            }
        }
        public static ColorRGBA8 Teal
        {
            get
            {
                return new ColorRGBA8(0xff808000);
            }
        }
        public static ColorRGBA8 Thistle
        {
            get
            {
                return new ColorRGBA8(0xffd8bfd8);
            }
        }
        public static ColorRGBA8 Tomato
        {
            get
            {
                return new ColorRGBA8(0xff4763ff);
            }
        }
        public static ColorRGBA8 Turquoise
        {
            get
            {
                return new ColorRGBA8(0xffd0e040);
            }
        }
        public static ColorRGBA8 Violet
        {
            get
            {
                return new ColorRGBA8(0xffee82ee);
            }
        }
        public static ColorRGBA8 Wheat
        {
            get
            {
                return new ColorRGBA8(0xffb3def5);
            }
        }
        public static ColorRGBA8 White
        {
            get
            {
                return new ColorRGBA8(uint.MaxValue);
            }
        }
        public static ColorRGBA8 WhiteSmoke
        {
            get
            {
                return new ColorRGBA8(0xfff5f5f5);
            }
        }
        public static ColorRGBA8 Yellow
        {
            get
            {
                return new ColorRGBA8(0xff00ffff);
            }
        }
        public static ColorRGBA8 YellowGreen
        {
            get
            {
                return new ColorRGBA8(0xff32cd9a);
            }
        }

        #endregion

    }
}
