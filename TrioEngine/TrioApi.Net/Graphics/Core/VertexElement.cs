using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioApi.Net.Graphics.Core
{
    [StructLayout(LayoutKind.Sequential)]
    public struct VertexElement
    {
        public int Offset;
        public VertexElementFormat Format;
        public VertexElementUsage Usage;
        public int UsageIndex;

        public VertexElement(int offset, VertexElementFormat elementFormat, VertexElementUsage elementUsage, int usageIndex)
        {
            Offset = offset;
            UsageIndex = usageIndex;
            Format = elementFormat;
            Usage = elementUsage;
        }
    }
}
