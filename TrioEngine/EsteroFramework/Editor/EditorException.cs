using System;

namespace EsteroFramework.Editor
{
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
