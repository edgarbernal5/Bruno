using System;
using System.IO;
using System.Reflection;

namespace Bruno.Logging
{
    public class FileLog : LogBase
    {
        private string m_targetFilename;
        public FileLog(string filename)
        {
            var exePath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            m_targetFilename = Path.Combine(exePath, filename);
        }

        public override void Debug(string format)
        {
            using (StreamWriter writer = File.AppendText(m_targetFilename))
            {
                LogMessage(GetLogMessage(format, LogType.DEBUG), writer);
            }
        }

        public override void Debug(string format, params object[] args)
        {
            using (StreamWriter writer = File.AppendText(m_targetFilename))
            {
                LogMessage(GetLogMessage(format, LogType.DEBUG, args), writer);
            }
        }

        public override void Info(string format, params object[] args)
        {
            using (StreamWriter writer = File.AppendText(m_targetFilename))
            {
                LogMessage(GetLogMessage(format, LogType.INFO, args), writer);
            }
        }

        public override void Error(Exception exception)
        {
            using (StreamWriter writer = File.AppendText(m_targetFilename))
            {
                LogMessage(GetLogMessage(exception.ToString(), LogType.ERROR), writer);
            }
        }

        public override void Warn(string format, params object[] args)
        {
            using (StreamWriter writer = File.AppendText(m_targetFilename))
            {
                LogMessage(GetLogMessage(format, LogType.WARN, args), writer);
            }
        }

        private void LogMessage(string message, TextWriter txtWriter)
        {
            try
            {
                txtWriter.WriteLine(message);
            }
            catch (Exception ex)
            {
            }
        }
    }
}