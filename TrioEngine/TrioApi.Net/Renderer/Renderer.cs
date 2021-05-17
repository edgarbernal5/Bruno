
using System;
using System.Runtime.InteropServices;
using TrioApi.Net.Graphics;
using TrioApi.Net.Maths;

namespace TrioApi.Net.Renderer
{
    public static class Renderer
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct Camera
        {
            public Vector3 m_position;
            public Vector3 m_target;
            public Vector3 m_up;

            public float m_nearPlane;
            public float m_farPlane;
            public float m_fieldOfView;
            public float m_aspectRatio;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Renderer_DrawScene", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_DrawScene(Camera camera);

        public static void DrawScene(Camera camera)
        {
            Internal_DrawScene(camera);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Renderer_Initialize", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Initialize(IntPtr device);

        public static void Initialize(GraphicsDevice device)
        {
            Internal_Initialize(device.NativePointer);
        }

        public static void UpdatePerFrameData()
        {

        }


    }
}
