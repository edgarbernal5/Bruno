using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Net.Graphics.Core
{
    public class DepthStencilState
    {
        #region Pinvokes

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "DepthStencilState_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor();

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "DepthStencilState_Default", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Default();

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "DepthStencilState_DepthRead", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_DepthRead();

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "DepthStencilState_None", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_None();
        
        #endregion

        internal IntPtr m_nativePtr;

        public DepthStencilState()
        {
            m_nativePtr = Internal_Ctor();
        }

        internal DepthStencilState(IntPtr native)
        {
            m_nativePtr = native;
        }

        static DepthStencilState()
        {
            Default = new DepthStencilState(Internal_Default());
            DepthRead = new DepthStencilState(Internal_DepthRead());
            None = new DepthStencilState(Internal_None());
        }

        public static DepthStencilState Default { get; private set; }
        public static DepthStencilState DepthRead { get; private set; }
        public static DepthStencilState None { get; private set; }
    }
}
