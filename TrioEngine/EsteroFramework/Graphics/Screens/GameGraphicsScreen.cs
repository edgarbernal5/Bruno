using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EsteroFramework.Graphics
{
    public abstract class GameGraphicsScreen
    {
        public string Name { get; set; }

        public bool IsVisible { get; set; }

        public GameGraphicsScreen()
        {
            IsVisible = true;
        
        }

        public abstract void Update(TimeSpan deltaTime);

        public abstract void Render(RenderContext renderContext);
    }
}
