using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioApi.Net
{
    public struct Rectangle : IEquatable<Rectangle>
    {
        public int X;
        public int Y;
        public int Width;
        public int Height;

        public int Left
        {
            get { return this.X; }
        }

        public int Right
        {
            get { return (this.X + this.Width); }
        }

        public int Top
        {
            get { return this.Y; }
        }

        public int Bottom
        {
            get { return (this.Y + this.Height); }
        }

        public bool IsEmpty
        {
            get
            {
                return ((((this.Width == 0) && (this.Height == 0)) && (this.X == 0)) && (this.Y == 0));
            }
        }

        public Rectangle(int x, int y, int width, int height)
        {
            this.X = x;
            this.Y = y;
            this.Width = width;
            this.Height = height;
        }

        public bool Equals(Rectangle other)
        {
            return this.X == other.X && this.Y == other.Y && this.Width == other.Width && this.Height == other.Height;
        }

        public override bool Equals(object obj)
        {
            bool result = false;
            if (obj is Rectangle)
            {
                result = this.Equals((Rectangle)obj);
            }
            return result;
        }
        public override int GetHashCode()
        {
            return this.X.GetHashCode() + this.Y.GetHashCode() + this.Width.GetHashCode() + this.Height.GetHashCode();
        }

        public static bool operator ==(Rectangle a, Rectangle b)
        {
            return a.X == b.X && a.Y == b.Y && a.Width == b.Width && a.Height == b.Height;
        }

        public static bool operator !=(Rectangle a, Rectangle b)
        {
            return a.X != b.X || a.Y != b.Y || a.Width != b.Width || a.Height != b.Height;
        }
    }
}
