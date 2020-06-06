using System;
using System.Diagnostics;

namespace Estero.Logging
{
    /// <summary>
    /// A simple logger thats logs everything to the debugger.
    /// </summary>
    public class DebugLog : ILog
    {
        public void Error(Exception exception)
        {
            Debug.WriteLine(CreateLogMessage(exception.ToString(), "ERROR"));
        }

        public void Info(string format, params object[] args)
        {
            Debug.WriteLine(CreateLogMessage(format, "INFO", args));
        }

        public void Warn(string format, params object[] args)
        {
            Debug.WriteLine(CreateLogMessage(format, "WARN", args));
        }

        private string CreateLogMessage(string format, string category, params object[] args)
        {
            return string.Format("[{0}] {1}",
                DateTime.Now.ToString("o"), string.Format(format, args));
        }
    }
}