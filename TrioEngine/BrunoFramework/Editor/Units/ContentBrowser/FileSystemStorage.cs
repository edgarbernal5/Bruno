using System;
using System.Collections.Generic;

namespace BrunoFramework.Editor.Units
{
    public class FileSystemStorage : Storage
    {
        public override IEnumerable<string> GetDirectories(string path)
        {
            throw new NotImplementedException();
        }

        public override IEnumerable<string> GetFiles(string path)
        {
            throw new NotImplementedException();
        }

        public override string GetRealPath(string path)
        {
            throw new NotImplementedException();
        }
    }
}
