using System;

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
            get { return X; }
        }

        public int Right
        {
            get { return (X + Width); }
        }

        public int Top
        {
            get { return Y; }
        }

        public int Bottom
        {
            get { return (Y + Height); }
        }

        public bool IsEmpty
        {
            get
            {
                return (Width == 0 && Height == 0 && X == 0 && Y == 0);
            }
        }

        public Rectangle(int x, int y, int width, int height)
        {
            X = x;
            Y = y;
            Width = width;
            Height = height;
        }

        public bool Equals(Rectangle other)
        {
            return X == other.X && Y == other.Y && Width == other.Width && Height == other.Height;
        }

        public override bool Equals(object obj)
        {
            bool result = false;
            if (obj is Rectangle)
            {
                result = Equals((Rectangle)obj);
            }
            return result;
        }

        public override int GetHashCode()
        {
            return X.GetHashCode() + Y.GetHashCode() + Width.GetHashCode() + Height.GetHashCode();
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
