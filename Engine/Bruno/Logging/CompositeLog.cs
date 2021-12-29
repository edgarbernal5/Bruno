using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bruno.Logging
{
    public class CompositeLog : ILog
    {
        private readonly IEnumerable<ILog> _logs;

        public CompositeLog(IEnumerable<ILog> logs)
        {
            _logs = logs;
        }

        public void Debug(string format, params object[] args)
        {
            foreach (var item in _logs)
            {
                item.Debug(format, args);
            }
        }

        public void Info(string format, params object[] args)
        {
            foreach (var item in _logs)
            {
                item.Info(format, args);
            }
        }

        public void Error(Exception exception)
        {
            foreach (var item in _logs)
            {
                item.Error(exception);
            }
        }

        public void Warn(string format, params object[] args)
        {
            foreach (var item in _logs)
            {
                item.Warn(format, args);
            }
        }
    }
}
