﻿using Estero.Collections;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Net.Graphics.Core
{
    public class EffectPass : INamedItem
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "EffectPass_GetName", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.LPStr)]
        private static extern string Internal_GetName(IntPtr pass);

        public string Name { get; private set; }

        internal IntPtr m_nativePtr;

        internal EffectPass(IntPtr nativePtr)
        {
            m_nativePtr = nativePtr;
            Name = Internal_GetName(m_nativePtr);
        }
    }
}
