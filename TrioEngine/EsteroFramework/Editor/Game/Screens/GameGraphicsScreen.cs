
using EsteroFramework.Graphics;
using EsteroFramework.Graphics.Data;
using System;
using TrioApi.Net.Game;

namespace EsteroFramework.Editor.Graphics
{
    public abstract class GameGraphicsScreen
    {
        public bool IsVisible { get; set; }

        public string Name { get; set; }

        public Scene Scene { get; set; }

        public Camera Camera { get; set; }

        public GameGraphicsScreen()
        {
            IsVisible = true;
        }

        public abstract void Update(TimeSpan deltaTime);

        public abstract void Render(RenderContext renderContext);
    }
}
