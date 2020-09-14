
using EsteroFramework.Graphics;
using System;
using TrioWpfFramework.Net.Game;

namespace EsteroFramework.Editor.Graphics
{
    public abstract class GameGraphicsScreen
    {
        public bool IsVisible { get; set; }

        public string Name { get; set; }

        public Scene Scene { get; set; }

        public GameGraphicsScreen()
        {
            IsVisible = true;
            Scene = null;
        }

        public abstract void Update(TimeSpan deltaTime);

        public abstract void Render(RenderContext renderContext);
    }
}
