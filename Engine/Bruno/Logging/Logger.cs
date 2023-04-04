using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Reflection;

namespace Bruno.Logging
{
    public class Logger
    {
        static readonly NullCompositeLog NullLogInstance = new NullCompositeLog();

        public static Func<string, CompositeLog> Targets = type => NullLogInstance;

        private string m_name;

        private IEnumerable<ILog> m_logs;

        private LogLevel m_logLevel;

        public void Debug(string message)
        {
            Log(LogLevel.Debug, message);
        }

        private void Log(LogLevel debug, string message)
        {

        }

        public static CompositeLog GetLog()
        {
            return Targets(GetClassFullName());
        }

        public static CompositeLog GetLog(string typeName)
        {
            return Targets(typeName);
        }

        public static string GetClassFullName(int skipFrmes = 2)
        {
            int num = skipFrmes;
            MethodBase method;
            string result;
            while (true)
            {
                StackFrame stackFrame = new StackFrame(num, false);
                method = stackFrame.GetMethod();
                Type declaringType = method.DeclaringType;
                if (declaringType == null)
                {
                    break;
                }
                num++;
                result = declaringType.FullName;
                if (!declaringType.Module.Name.Equals("mscorlib.dll", StringComparison.OrdinalIgnoreCase))
                {
                    return result;
                }
            }
            result = method.Name;
            return result;
        }
    }
}
