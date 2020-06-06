using System;
using System.IO;
using System.Reflection;

namespace Estero.Logging
{
    public class FileLog : ILog
    {
        private string m_targetFilename;
        public FileLog(string filename)
        {
            var exePath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            m_targetFilename = Path.Combine(exePath, filename);
        }

        public void Debug(string format, params object[] args)
        {
            using (StreamWriter writer = File.AppendText(m_targetFilename))
            {
                LogMessage(CreateLogMessage(format, "DEBUG", args), writer);
            }
        }

        public void Info(string format, params object[] args)
        {
            using (StreamWriter writer = File.AppendText(m_targetFilename))
            {
                LogMessage(CreateLogMessage(format, "INFO", args), writer);
            }
        }

        public void Error(Exception exception)
        {
            using (StreamWriter writer = File.AppendText(m_targetFilename))
            {
                LogMessage(CreateLogMessage(exception.ToString(), "ERROR"), writer);
            }
        }

        public void Warn(string format, params object[] args)
        {
            using (StreamWriter writer = File.AppendText(m_targetFilename))
            {
                LogMessage(CreateLogMessage(format, "WARN", args), writer);
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

        private string CreateLogMessage(string format, string category, params object[] args)
        {
            return string.Format("[{0}] {1}",
                DateTime.Now.ToString("o"), string.Format(format, args));
        }
    }
}
