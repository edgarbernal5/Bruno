using System;

namespace Bruno.Logging
{
    public class NullCompositeLog : CompositeLog
    {
        public NullCompositeLog()
            : base(null)
        {
        }

        public override void Debug(string message)
        {
        }

        public override void Debug(string format, params object[] args)
        {
        }

        public override void Error(Exception exception)
        {
        }

        public override void Info(string format, params object[] args)
        {
        }

        public override void Warn(string format, params object[] args)
        {
        }
    }
}
