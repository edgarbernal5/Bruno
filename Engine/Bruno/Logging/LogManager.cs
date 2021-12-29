using System;
using System.Diagnostics;
using System.Reflection;

namespace Bruno.Logging
{
    public static class LogManager
    {
        static readonly ILog NullLogInstance = new NullLog();

        public static Func<string, ILog> Targets = type => NullLogInstance;

        public static ILog GetLog()
        {
			return Targets(GetClassFullName());
		}

		internal static string GetClassFullName(int skipFrmes = 2)
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
