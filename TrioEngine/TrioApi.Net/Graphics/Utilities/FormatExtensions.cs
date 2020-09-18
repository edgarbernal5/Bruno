using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrioApi.Net.Graphics;

namespace TrioApi.Net.Graphics.Utilities
{
    public static class FormatExtensions
    {
		public static int GetByteSize(this SurfaceFormat format)
		{
			switch (format)
			{
				case SurfaceFormat.Dxt1:
				case SurfaceFormat.Dxt1SRgb:
				case SurfaceFormat.Dxt1a:
				case SurfaceFormat.RgbPvrtc2Bpp:
				case SurfaceFormat.RgbaPvrtc2Bpp:
				case SurfaceFormat.RgbPvrtc4Bpp:
				case SurfaceFormat.RgbaPvrtc4Bpp:
				case SurfaceFormat.RgbEtc1:
					// One texel in DXT1, PVRTC (2bpp and 4bpp) and ETC1 is a minimum 4x4 block (8x4 for PVRTC 2bpp), which is 8 bytes
					return 8;
				case SurfaceFormat.Dxt3:
				case SurfaceFormat.Dxt3SRgb:
				case SurfaceFormat.Dxt5:
				case SurfaceFormat.Dxt5SRgb:
				case SurfaceFormat.RgbaAtcExplicitAlpha:
				case SurfaceFormat.RgbaAtcInterpolatedAlpha:
					//  One texel in DXT3 and DXT5 is a minimum 4x4 block, which is 16 bytes
					return 16;
				case SurfaceFormat.Alpha8:
					return 1;
				case SurfaceFormat.Bgr565:
				case SurfaceFormat.Bgra4444:
				case SurfaceFormat.Bgra5551:
				case SurfaceFormat.HalfSingle:
				case SurfaceFormat.NormalizedByte2:
					return 2;
				case SurfaceFormat.Color:
				case SurfaceFormat.ColorSRgb:
				case SurfaceFormat.Single:
				case SurfaceFormat.Rg32:
				case SurfaceFormat.HalfVector2:
				case SurfaceFormat.NormalizedByte4:
				case SurfaceFormat.Rgba1010102:
				case SurfaceFormat.Bgra32:
				case SurfaceFormat.Bgra32SRgb:
				case SurfaceFormat.Bgr32:
				case SurfaceFormat.Bgr32SRgb:
					return 4;
				case SurfaceFormat.HalfVector4:
				case SurfaceFormat.Rgba64:
				case SurfaceFormat.Vector2:
					return 8;
				case SurfaceFormat.Vector4:
					return 16;
				default:
					return 0;
			}
		}
	}
}
