using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioApi.Net.Game
{
    public class Component : Object
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Component_GetEnabled", CallingConvention = CallingConvention.StdCall)]
        private static extern bool Internal_GetEnabled(IntPtr component);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Component_SetEnabled", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetEnabled(IntPtr component, bool enabled);

        public bool Enabled
        {
            get
            {
                return Internal_GetEnabled(m_nativePtr);
            }
            set
            {
                Internal_SetEnabled(m_nativePtr, value);
            }
        }

        internal Component(IntPtr nativePtr) 
            : base(nativePtr)
        {
        }
    }
}
