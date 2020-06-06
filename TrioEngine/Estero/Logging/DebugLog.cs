using System;

namespace Estero.Logging
{
    public class DebugLog : ILog
    {
        public void Debug(string format, params object[] args)
        {
            System.Diagnostics.Debug.WriteLine(CreateLogMessage(format, "DEBUG", args));
        }

        public void Info(string format, params object[] args)
        {
            System.Diagnostics.Debug.WriteLine(CreateLogMessage(format, "INFO", args));
        }
        public void Error(Exception exception)
        {
            System.Diagnostics.Debug.WriteLine(CreateLogMessage(exception.ToString(), "ERROR"));
        }

        public void Warn(string format, params object[] args)
        {
            System.Diagnostics.Debug.WriteLine(CreateLogMessage(format, "WARN", args));
        }

        private string CreateLogMessage(string format, string category, params object[] args)
        {
            return string.Format("[{0}] {1}",
                DateTime.Now.ToString("o"), string.Format(format, args));
        }
    }
}