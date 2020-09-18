using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EsteroFramework.Graphics.Editor
{
    public interface IEditorPrimitivesService
    {
        GridMesh GridMesh { get; }

        void InitializeMeshes();
    }
}
