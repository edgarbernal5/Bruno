using System;
using System.Globalization;
using System.Runtime.InteropServices;
using BrunoApi.Net.Maths;

namespace BrunoApi.Net.Graphics.Core
{
    [StructLayout(LayoutKind.Sequential)]
    public struct VertexPositionColor : IVertexType
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "VertexDeclaration_GetPC", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetPC();

        public Vector3 Position;
        public ColorRGBA8 Color;

        public static readonly VertexDeclaration VertexDeclaration;

        VertexDeclaration IVertexType.VertexDeclaration
        {
            get
            {
                return VertexPositionColor.VertexDeclaration;
            }
        }

        public VertexPositionColor(Vector3 position, ColorRGBA8 color)
        {
            Position = position;
            Color = color;
        }

        public override int GetHashCode()
        {
            return Helpers.SmartGetHashCode(this);
        }

        public override string ToString()
        {
            return string.Format(CultureInfo.CurrentCulture, "{{Position:{0} Color:{1}}}", new object[] { this.Position, this.Color });
        }

        public static bool operator ==(VertexPositionColor left, VertexPositionColor right)
        {
            return ((left.Color == right.Color) && (left.Position == right.Position));
        }

        public static bool operator !=(VertexPositionColor left, VertexPositionColor right)
        {
            return !(left == right);
        }

        public override bool Equals(object obj)
        {
            if (obj == null)
            {
                return false;
            }
            if (obj.GetType() != base.GetType())
            {
                return false;
            }
            return (this == ((VertexPositionColor)obj));
        }

        static VertexPositionColor()
        {
            //VertexElement[] elements = new VertexElement[] {
            //    new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position, 0),
            //    new VertexElement(12, VertexElementFormat.Color, VertexElementUsage.Color, 0)
            //};
            VertexDeclaration declaration = new VertexDeclaration(Internal_GetPC());
            //declaration.Name = "VertexPositionColor.VertexDeclaration";
            VertexDeclaration = declaration;
        }
    }
}
