
using BrunoFramework.Graphics.Interop;
using System.Collections.Generic;
using BrunoApi.Net.Graphics;

namespace BrunoFramework.Graphics
{
    public interface IGraphicsService
    {
        GraphicsDevice GraphicsDevice { get; }

        List<IGameSurfaceTarget> GameSurfaceTargets { get; }
    }
}
