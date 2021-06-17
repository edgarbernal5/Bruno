
using System.Runtime.InteropServices;
using BrunoApi.Net.Maths;

namespace BrunoApi.Net.Graphics
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Viewport
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
    }
}
