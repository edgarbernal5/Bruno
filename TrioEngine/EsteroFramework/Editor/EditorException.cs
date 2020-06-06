using System;

namespace EsteroFramework.Editor
{
    /// <summary>
    /// The exception that is raised when an error occurs during execution of the editor.
    /// </summary>
    [Serializable]
    public class EditorException : Exception
    {
        public EditorException()
        {
        }

        public EditorException(string message)
            : base(message)
        {
        }

        public EditorException(string message, Exception innerException)
            : base(message, innerException)
        {
        }
    }
}
