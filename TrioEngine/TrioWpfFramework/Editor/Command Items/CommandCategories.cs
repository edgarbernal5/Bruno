using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Editor
{
    /// <summary>
    /// Defines the default command categories.
    /// </summary>
    public static class CommandCategories
    {
        /// <summary>
        /// The default command category.
        /// </summary>
        public static readonly string Default = "Misc";


        /// <summary>
        /// Command related to the Edit menu.
        /// </summary>
        public static readonly string Edit = "Edit";


        /// <summary>
        /// Command related to the File menu.
        /// </summary>
        public static readonly string File = "File";


        /// <summary>
        /// Command related the Help menu.
        /// </summary>
        public static readonly string Help = "Help";


        /// <summary>
        /// Command related to search operations.
        /// </summary>
        public static readonly string Search = "Search";


        /// <summary>
        /// Command related to the Tools menu.
        /// </summary>
        public static readonly string Tools = "Tools";


        /// <summary>
        /// Command related to the View menu.
        /// </summary>
        public static readonly string View = "View";


        /// <summary>
        /// Command related to the Window menu.
        /// </summary>
        public static readonly string Window = "Window";
    }
}
