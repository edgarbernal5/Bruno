
using System;
using System.Runtime.InteropServices;
using TrioApi.Net.Graphics;
using TrioApi.Net.Maths;

namespace TrioApi.Net.Renderer
{
    public static class Renderer
    {
        public struct Camera
        {
            Vector3 m_position;
            Vector3 m_target;
            Vector3 m_up;

            float m_nearPlane;
            float m_farPlane;
            float m_fieldOfView;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Renderer_Initialize", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Initialize(IntPtr device);


        public static void DrawScene(Camera camera)
        {
        }

        public static void Initialize(GraphicsDevice device)
        {
            Internal_Initialize(device.NativePointer);
        }

        public static void UpdatePerFrameData()
        {

        }


    }
}
