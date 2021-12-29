using System;

namespace Bruno.Logging
{
    public class NullLog : ILog
    {
        public void Debug(string format, params object[] args)
        {
        }

        public void Error(Exception exception)
        {
        }

        public void Info(string format, params object[] args)
        {
        }

        public void Warn(string format, params object[] args)
        {
        }
    }
}
