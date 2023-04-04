using System;

namespace Bruno.Logging
{
    //ConfigSectionHandler

    public interface ILog
    {
        void Debug(string message);

        void Debug(string format, params object[] args);

        void Info(string format, params object[] args);

        void Warn(string format, params object[] args);

        void Error(Exception exception);
    }
}
