using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using TrioApi.Net.Math;

namespace TrioApi.Net.Graphics.Core
{
    [StructLayout(LayoutKind.Sequential)]
    public struct VertexPosition : IVertexType
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "VertexDeclaration_GetP", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetP();

        public Vector3 Position;

        public static readonly VertexDeclaration VertexDeclaration;

        VertexDeclaration IVertexType.VertexDeclaration
        {
            get
            {
                return VertexPosition.VertexDeclaration;
            }
        }

        public VertexPosition(Vector3 position)
        {
            this.Position = position;
        }

        public override int GetHashCode()
        {
            return Helpers.SmartGetHashCode(this);
        }

        public override string ToString()
        {
            return string.Format(CultureInfo.CurrentCulture, "{{Position:{0}}}", new object[] { this.Position });
        }

        public static bool operator ==(VertexPosition left, VertexPosition right)
        {
            return ((left.Position == right.Position));
        }

        public static bool operator !=(VertexPosition left, VertexPosition right)
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
            return (this == ((VertexPosition)obj));
        }

        static VertexPosition()
        {
            //VertexElement[] elements = new VertexElement[] {
            //    new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position, 0)
            //};
            VertexDeclaration declaration = new VertexDeclaration(Internal_GetP());
            //declaration.Name = "VertexPosition.VertexDeclaration";
            VertexDeclaration = declaration;
        }
    }
}
