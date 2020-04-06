using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Editor
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
        protected EditorException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
    }
}
