
using BrunoApi.Net.Graphics.Core;

namespace BrunoFramework.Graphics.Editor
{
    public interface IEditorPrimitivesService
    {
        GridMesh GridMesh { get; }
        Effect LineEffect { get; }

        void InitializeMeshes();
    }
}
