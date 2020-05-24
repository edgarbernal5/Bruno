
namespace TrioWpfFramework.Editor.Themes
{
    /// <summary>
    /// Represents the message that is broadcast over the <see cref="IMessageBus"/> when the UI
    /// theme changed.
    /// </summary>
    internal class ThemeMessage
    {
        /// <summary>
        /// Gets the name of the theme.
        /// </summary>
        /// <value>The name theme.</value>
        public string Theme { get; }


        /// <summary>
        /// Initializes a new instance of the <see cref="ThemeMessage"/> class.
        /// </summary>
        /// <param name="theme">The name of the UI theme. Can be <see langword="null"/>.</param>
        public ThemeMessage(string theme)
        {
            Theme = theme;
        }
    }
}
