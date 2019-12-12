using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioApi.Net.Graphics.Core
{
    public class BlendState
    {
        #region Pinvokes

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "BlendState_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor();

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "BlendState_Additive", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Additive();

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "BlendState_AlphaBlend", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_AlphaBlend();

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "BlendState_NonPremultiplied", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_NonPremultiplied();

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "BlendState_Opaque", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Opaque();

        #endregion

        internal IntPtr m_nativePtr;

        public BlendState()
        {
            m_nativePtr = Internal_Ctor();
        }

        internal BlendState(IntPtr native)
        {
            m_nativePtr = native;
        }

        static BlendState()
        {
            Additive = new BlendState(Internal_Additive());
            AlphaBlend = new BlendState(Internal_AlphaBlend());
            NonPremultiplied = new BlendState(Internal_NonPremultiplied());
            Opaque = new BlendState(Internal_Opaque());
        }

        public static BlendState Additive { get; private set; }
        public static BlendState AlphaBlend { get; private set; }
        public static BlendState NonPremultiplied { get; private set; }
        public static BlendState Opaque { get; private set; }

    }
}
