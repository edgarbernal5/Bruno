
using System.Runtime.InteropServices;

namespace BrunoApi.Net.Graphics.Core
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
