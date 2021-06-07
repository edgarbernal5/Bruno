
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Maths
{
    [StructLayout(LayoutKind.Sequential)]
    public struct BoundingBox
    {
        public Vector3 Center;
        public Vector3 Extents;

        public BoundingBox(Vector3 center, Vector3 extents)
        {
            Center = center;
            Extents = extents;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "BoundingBox_IntersectsRay", CallingConvention = CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.U1)]
        private static extern bool Internal_IntersectsRay(ref BoundingBox box, ref Ray ray, ref float distance);

        public float? Intersects(Ray ray)
        {
            float distance = 0.0f;
            bool result = Internal_IntersectsRay(ref this, ref ray, ref distance);

            return result ? new float?(distance) : null;
        }
    }
}
