
using System.Collections.Generic;

namespace BrunoFramework.Editor.Units
{
    public interface IProjectFileService
    {
        List<ProjectFileFactory> Factories { get; }

        ProjectFile New(ProjectFileType projectFileType);
        ProjectFile Open(string filename);
    }
}
