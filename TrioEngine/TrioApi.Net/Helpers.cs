﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioApi.Net
{
    internal static class Helpers
    {
        public static unsafe int SmartGetHashCode(object obj)
        {
            int num3;
            GCHandle handle = GCHandle.Alloc(obj, GCHandleType.Pinned);
            try
            {
                int num4 = Marshal.SizeOf(obj);
                int num2 = 0;
                int num = 0;
                for (int* numPtr = (int*)handle.AddrOfPinnedObject().ToPointer(); (num2 + 4) <= num4; numPtr++)
                {
                    num ^= numPtr[0];
                    num2 += 4;
                }
                num3 = (num == 0) ? 0x7fffffff : num;
            }
            finally
            {
                handle.Free();
            }
            return num3;
        }
    }
}
