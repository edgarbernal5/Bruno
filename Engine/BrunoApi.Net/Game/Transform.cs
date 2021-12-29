
using Estero.Interop;
using System;
using System.Runtime.InteropServices;
using TrioApi.Net.Maths;

namespace TrioApi.Net.Game
{
    public class Transform : CppObject
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor();

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_GetParent", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetParent(IntPtr transform);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_GetPosition", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetPosition(IntPtr transform, ref Vector3 position);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_GetRotation", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetRotation(IntPtr transform, ref Quaternion rotation);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_GetScale", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetScale(IntPtr transform, ref Vector3 scale);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_GetLocalPosition", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetLocalPosition(IntPtr transform, ref Vector3 position);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_GetLocalRotation", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetLocalRotation(IntPtr transform, ref Quaternion rotation);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_GetLocalScale", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetLocalScale(IntPtr transform, ref Vector3 scale);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_SetPosition", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetPosition(IntPtr transform, ref Vector3 position);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_SetRotation", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetRotation(IntPtr transform, ref Quaternion rotation);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_SetScale", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetScale(IntPtr transform, ref Vector3 scale);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_SetLocalPosition", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetLocalPosition(IntPtr transform, ref Vector3 position);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_SetLocalRotation", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetLocalRotation(IntPtr transform, ref Quaternion rotation);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_SetLocalScale", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetLocalScale(IntPtr transform, ref Vector3 scale);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Transform_SetParent", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetParent(IntPtr transform, IntPtr parent);
                
        public Vector3 Position
        {
            get
            {
                Vector3 position = Vector3.Zero;
                Internal_GetPosition(m_nativePointer, ref position);
                return position;
            }
            set
            {
                Internal_SetPosition(m_nativePointer, ref value);
            }
        }

        public Quaternion Rotation
        {
            get
            {
                Quaternion rotation = Quaternion.Identity;
                Internal_GetRotation(m_nativePointer, ref rotation);
                return rotation;
            }
            set
            {
                Internal_SetRotation(m_nativePointer, ref value);
            }
        }

        public Vector3 Scale
        {
            get
            {
                Vector3 scale = Vector3.Zero;
                Internal_GetScale(m_nativePointer, ref scale);
                return scale;
            }
            set
            {
                Internal_SetScale(m_nativePointer, ref value);
            }
        }

        public Vector3 LocalPosition
        {
            get
            {
                Vector3 position = Vector3.Zero;
                Internal_GetLocalPosition(m_nativePointer, ref position);
                return position;
            }
            set
            {
                Internal_SetLocalPosition(m_nativePointer, ref value);
            }
        }

        public Quaternion LocalRotation
        {
            get
            {
                Quaternion rotation = Quaternion.Identity;
                Internal_GetLocalRotation(m_nativePointer, ref rotation);
                return rotation;
            }
            set
            {
                Internal_SetLocalRotation(m_nativePointer, ref value);
            }
        }

        public Transform Parent
        {
            get
            {
                return null;
            }
            set
            {

            }
        }

        public Vector3 LocalScale
        {
            get
            {
                Vector3 scale = Vector3.Zero;
                Internal_GetLocalScale(m_nativePointer, ref scale);
                return scale;
            }
            set
            {
                Internal_SetLocalScale(m_nativePointer, ref value);
            }
        }

        internal Transform(IntPtr nativePtr)
        {
            m_nativePointer = nativePtr;
        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {

            }
        }
    }
}
