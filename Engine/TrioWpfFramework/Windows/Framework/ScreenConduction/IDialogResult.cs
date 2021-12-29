
namespace TrioWpfFramework.Windows.Framework
{
    /// <summary>
    /// Represents an item which provides the result of a dialog box.
    /// </summary>
    public interface IDialogResult
    {
        /// <summary>
        /// Gets a value that indicates whether the dialog box was accepted or canceled.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if the dialog box was accepted; <see langword="false"/> if the
        /// dialog box was canceled. The default is <see langword="null"/>.
        /// </value>
        /// <remarks>
        /// Setting the dialog result automatically closes the dialog!
        /// </remarks>
        bool? DialogResult { get; }
    }
}
