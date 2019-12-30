using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using TrioApi.Net.Maths;

namespace TrioApi.Net.Graphics.Core
{
    [StructLayout(LayoutKind.Sequential)]
    public struct VertexPositionNormalTexture : IVertexType
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "VertexDeclaration_GetPNT", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetPNT();

        public Vector3 Position;
        public Vector3 Normal;
        public Vector2 TextureCoordinate;

        public static readonly VertexDeclaration VertexDeclaration;

        VertexDeclaration IVertexType.VertexDeclaration
        {
            get
            {
                return VertexPositionNormalTexture.VertexDeclaration;
            }
        }

        public VertexPositionNormalTexture(Vector3 position, Vector3 normal, Vector2 texture)
        {
            this.Position = position;
            this.Normal = normal;
            this.TextureCoordinate = texture;
        }

        public override int GetHashCode()
        {
            return Helpers.SmartGetHashCode(this);
        }

        public override string ToString()
        {
            return string.Format(CultureInfo.CurrentCulture, "{{Position:{0} Normal:{1} TextureCoordinate:{2}}}", new object[] { this.Position, this.Normal, this.TextureCoordinate });
        }

        public static bool operator ==(VertexPositionNormalTexture left, VertexPositionNormalTexture right)
        {
            return ((left.Normal == right.Normal) && (left.Position == right.Position) && (left.TextureCoordinate == right.TextureCoordinate));
        }

        public static bool operator !=(VertexPositionNormalTexture left, VertexPositionNormalTexture right)
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
            return (this == ((VertexPositionNormalTexture)obj));
        }

        static VertexPositionNormalTexture()
        {
            //VertexElement[] elements = new VertexElement[]
            //{
            //    new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position, 0),
            //    new VertexElement(12, VertexElementFormat.Vector3, VertexElementUsage.Normal, 0),
            //    new VertexElement(24, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 0)
            //};
            VertexDeclaration declaration = new VertexDeclaration(Internal_GetPNT());
            //declaration.Name = "VertexPositionNormalTexture.VertexDeclaration";
            VertexDeclaration = declaration;
        }
    }
}
