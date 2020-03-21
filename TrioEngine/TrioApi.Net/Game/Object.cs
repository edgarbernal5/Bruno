using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Net.Game
{
    public class Object
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Object_GetId", CallingConvention = CallingConvention.StdCall)]
        private static extern int Internal_GetId(IntPtr objectPtr);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Object_GetName", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.LPStr)]
        private static extern string Internal_GetName(IntPtr objectPtr);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Object_SetName", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern void Internal_SetName(IntPtr objectPtr, [MarshalAs(UnmanagedType.LPStr)] string name);
        
        internal IntPtr m_nativePtr;

        public int Id
        {
            get
            {
                return Internal_GetId(m_nativePtr);
            }
        }

        public string Name
        {
            get
            {
                return Internal_GetName(m_nativePtr);
            }
            set
            {
                Internal_SetName(m_nativePtr, value);
            }
        }

        protected Object(IntPtr nativePtr)
        {
            m_nativePtr = nativePtr;
        }
    }
}
