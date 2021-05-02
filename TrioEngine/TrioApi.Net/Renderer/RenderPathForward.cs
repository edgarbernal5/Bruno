using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using TrioApi.Net.Game;

namespace TrioApi.Net.Renderer
{
    public class RenderPathForward : RenderPath
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "RenderPathForward_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor();

        public RenderPathForward(Scene scene) 
            : base(scene)
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

        }
    }
}
