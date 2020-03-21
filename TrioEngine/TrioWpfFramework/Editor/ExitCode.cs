using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Editor
{
    /// <summary>
    /// Defines the Win32 system error codes.
    /// </summary>
    /// <remarks>
    /// Reference:
    /// <seealso href="https://msdn.microsoft.com/en-us/library/windows/desktop/ms681382.aspx"/>
    /// </remarks>
    public enum ExitCode
    {
        /// <summary>The operation completed successfully.</summary>
        ERROR_SUCCESS = 0,


        /// <summary>One or more arguments are not correct.</summary>
        ERROR_BAD_ARGUMENTS = 160,  // 0x0A0
    }
}
