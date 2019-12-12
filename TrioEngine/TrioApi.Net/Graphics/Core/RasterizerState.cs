using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioApi.Net.Graphics.Core
{
    public class RasterizerState
    {
        #region Pinvokes

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "RasterizerState_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor();

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "RasterizerState_CullClockwise", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_CullClockwise();

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "RasterizerState_CullCounterClockwise", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_CullCounterClockwise();

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "RasterizerState_CullNone", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_CullNone();

        #endregion

        internal IntPtr m_nativePtr;

        public RasterizerState()
        {
            m_nativePtr = Internal_ctor();
        }

        internal RasterizerState(IntPtr native)
        {
            m_nativePtr = native;
        }

        static RasterizerState()
        {
            CullClockwise = new RasterizerState(Internal_CullClockwise());
            CullCounterClockwise = new RasterizerState(Internal_CullCounterClockwise());
            CullNone = new RasterizerState(Internal_CullNone());
        }

        public static RasterizerState CullClockwise { get; private set; }
        public static RasterizerState CullCounterClockwise { get; private set; }
        public static RasterizerState CullNone { get; private set; }

    }
}
