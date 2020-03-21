using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Net.Graphics.Core
{
    /// <summary>Vertex type interface which is implemented by a custom vertex type structure.</summary>
	public interface IVertexType
    {
        /// <summary>Vertex declaration, which defines per-vertex data.</summary>
        VertexDeclaration VertexDeclaration
        {
            get;
        }
    }
}
