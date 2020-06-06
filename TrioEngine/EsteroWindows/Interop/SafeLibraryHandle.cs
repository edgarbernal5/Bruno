using Microsoft.Win32.SafeHandles;

namespace EsteroWindows.Interop
{
    public sealed class SafeLibraryHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        private SafeLibraryHandle()
            : base(true)
        {
        }


        /// <summary>
        /// When overridden in a derived class, executes the code required to free the handle.
        /// </summary>
        /// <returns>
        /// <see langword="true"/> if the handle is released successfully; otherwise, in the event
        /// of a catastrophic failure, <see langword="false"/>. In this case, it generates a
        /// releaseHandleFailed MDA Managed Debugging Assistant.
        /// </returns>
        protected override bool ReleaseHandle()
        {
            return Win32.FreeLibrary(handle);
        }
    }
}
