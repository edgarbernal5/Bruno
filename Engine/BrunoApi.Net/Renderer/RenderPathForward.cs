using System;
using System.Runtime.InteropServices;

namespace BrunoApi.Net.Renderer
{
    public class RenderPathForward : RenderPath
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "RenderPathForward_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor();

        public RenderPathForward() 
            : base()
        {
            m_nativePointer = Internal_ctor();
        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {

            }
        }

        public override void Render()
        {
            Internal_Render(m_nativePointer);
        }
    }
}
