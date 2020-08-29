using System;

namespace EsteroFramework.Editor.Timing
{
    public class GameTime
    {
        public TimeSpan ElapsedGameTime { get; set; }

        public TimeSpan TotalGameTime { get; set; }

        public GameTime()
        {
            ElapsedGameTime = TimeSpan.Zero;
            TotalGameTime = TimeSpan.Zero;
        }
    }
}
