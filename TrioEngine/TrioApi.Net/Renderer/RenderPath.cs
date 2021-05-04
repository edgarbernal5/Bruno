﻿
using Estero.Interop;
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Renderer
{
    public abstract class RenderPath : CppObject
    {
        protected RenderPath()
        {
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "RenderPath_Render", CallingConvention = CallingConvention.StdCall)]
        protected static extern void Internal_Render(IntPtr renderPath);

        public abstract void Render();
    }
}
