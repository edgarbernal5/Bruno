
using System;
using System.Collections.Generic;

namespace Bruno.Logging
{
    public class CompositeLog : ILog
    {
        public IEnumerable<ILog> Logs
        {
            get => m_logs;
        }
        private readonly IEnumerable<ILog> m_logs;

        public CompositeLog(IEnumerable<ILog> logs)
        {
            m_logs = logs;
        }

        public virtual void Debug(string message)
        {
            foreach (var item in m_logs)
            {
                item.Debug(message);
            }
        }

        public virtual void Debug(string format, params object[] args)
        {
            foreach (var item in m_logs)
            {
                item.Debug(format, args);
            }
        }

        public virtual void Info(string format, params object[] args)
        {
            foreach (var item in m_logs)
            {
                item.Info(format, args);
            }
        }

        public virtual void Error(Exception exception)
        {
            foreach (var item in m_logs)
            {
                item.Error(exception);
            }
        }

        public virtual void Warn(string format, params object[] args)
        {
            foreach (var item in m_logs)
            {
                item.Warn(format, args);
            }
        }
    }
}
