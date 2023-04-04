
using System;
using System.Collections.Generic;
using System.Text;

namespace Bruno.Logging
{
    public class InMemoryLog : LogBase
    {
        private const int MAX_NUMBER_OF_LOG_MESSAGES = 50000;
        private Queue<string> m_loggedMessages;

        public delegate void MessageEvent(string message);
        public event MessageEvent MessageWritten;

        public InMemoryLog()
        {
            m_loggedMessages = new Queue<string>();
        }

        private void LogToQueue(LogType logType, string message)
        {
            if (m_loggedMessages.Count >= MAX_NUMBER_OF_LOG_MESSAGES)
            {
                m_loggedMessages.Dequeue();
            }

            m_loggedMessages.Enqueue(message);
            MessageWritten?.Invoke(message);
        }

        public override void Debug(string message)
        {
            LogToQueue(LogType.DEBUG, GetLogMessage(message, LogType.DEBUG));
        }

        public override void Debug(string format, params object[] args)
        {
            LogToQueue(LogType.DEBUG, GetLogMessage(format, LogType.DEBUG, args));
        }

        public override void Error(Exception exception)
        {
            LogToQueue(LogType.ERROR, GetLogMessage(exception.ToString(), LogType.ERROR));
        }

        public override void Info(string format, params object[] args)
        {
            LogToQueue(LogType.INFO, GetLogMessage(format, LogType.INFO, args));
        }

        public override void Warn(string format, params object[] args)
        {
            LogToQueue(LogType.WARN, GetLogMessage(format, LogType.WARN, args));
        }

        public void FillWrittenLog(StringBuilder builder)
        {
            foreach (var message in m_loggedMessages)
            {
                builder.AppendLine(message);
            }
        }
    }
}
