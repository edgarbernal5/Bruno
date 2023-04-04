
using System;
using System.Runtime.InteropServices;
using BrunoApi.Net.Maths;

namespace BrunoApi.Net.Graphics
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Viewport : IEquatable<Viewport>
    {
        public float X;
        public float Y;
        public float Width;
        public float Height;
        public float MinDepth;
        public float MaxDepth;

        public Viewport(float x, float y, float width, float height)
        {
            X = x;
            Y = y;
            Width = width;
            Height = height;
            MinDepth = 0.0f;
            MaxDepth = 1.0f;
        }

        public float AspectRatio
        {
            get
            {
                if (Height == 0 || Width == 0)
                {
                    return 0.0f;
                }
                return Width / Height;
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Viewport_Project", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Project(ref Viewport viewport, ref Vector3 source, ref Matrix projection, ref Matrix view, ref Matrix world);

        public Vector3 Project(Vector3 source, Matrix projection, Matrix view, Matrix world)
        {
            Internal_Project(ref this, ref source, ref projection, ref view, ref world);
            return source;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Viewport_Unproject", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Unproject(ref Viewport viewport, ref Vector3 source, ref Matrix projection, ref Matrix view, ref Matrix world);

        public Vector3 Unproject(Vector3 source, Matrix projection, Matrix view, Matrix world)
        {
            Internal_Unproject(ref this, ref source, ref projection, ref view, ref world);
            return source;
        }

        public bool Equals(Viewport other)
        {
            return X == other.X && Y == other.Y && Width == other.Width && Height == other.Height
                && MaxDepth == other.MaxDepth && MinDepth == other.MinDepth;
        }

        public override bool Equals(object obj)
        {
            bool result = false;
            if (obj is Viewport)
            {
                result = Equals((Viewport)obj);
            }
            return result;
        }

        public static bool operator ==(Viewport value1, Viewport value2)
        {
            return value1.X == value2.X && value1.Y == value2.Y && value1.Width == value2.Width 
                && value1.Height == value2.Height && value1.MaxDepth == value2.MaxDepth && value1.MinDepth == value2.MinDepth;
        }

        public static bool operator !=(Viewport value1, Viewport value2)
        {
            return value1.X != value2.X || value1.Y != value2.Y || value1.Width == value2.Width
                || value1.Height != value2.Height || value1.MaxDepth != value2.MaxDepth || value1.MinDepth != value2.MinDepth;
        }

        public override int GetHashCode()
        {
            return X.GetHashCode() + Y.GetHashCode() + Width.GetHashCode() + Height.GetHashCode() + MaxDepth.GetHashCode() + MinDepth.GetHashCode();
        }
    }
}
