
using EsteroFramework.Graphics.Interop;
using System.Collections.Generic;
using System.Windows.Documents;
using TrioWpfFramework.Net.Graphics;

namespace EsteroFramework.Graphics
{
    public interface IGraphicsService
    {
        GraphicsDevice GraphicsDevice { get; }

        List<IGameSurfaceTarget> GameSurfaceTargets { get; }
    }
}
