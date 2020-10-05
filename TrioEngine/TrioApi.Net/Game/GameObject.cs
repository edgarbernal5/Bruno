using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioApi.Net.Game
{
    public class GameObject : Object
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GameObject_Create", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Create([MarshalAs(UnmanagedType.LPStr)] string name);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GameObject_IsActiveInTree", CallingConvention = CallingConvention.StdCall)]
        private static extern bool Internal_IsActiveInTree(IntPtr gameObject);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GameObject_IsActiveSelf", CallingConvention = CallingConvention.StdCall)]
        private static extern bool Internal_IsActiveSelf(IntPtr gameObject);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GameObject_SetActiveSelf", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetActiveSelf(IntPtr gameObject, bool active);

        public bool ActiveSelf
        {
            get
            {
                return Internal_IsActiveSelf(m_nativePointer);
            }
            set
            {
                Internal_SetActiveSelf(m_nativePointer, value);
            }
        }


        public bool ActiveInTree
        {
            get
            {
                return Internal_IsActiveInTree(m_nativePointer);
            }
        }

        internal GameObject(IntPtr nativePtr)
            : base(nativePtr)
        {
        }

        public static GameObject Create(string name)
        {
            return new GameObject(Internal_Create(name));
        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
            }
        }
    }
}
