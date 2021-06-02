
using System.Collections.Generic;

namespace EsteroFramework.Editor.Units
{
    public interface IProjectFileService
    {
        List<ProjectFileFactory> Factories { get; }

        ProjectFile New(ProjectFileType projectFileType);
        ProjectFile Open(string filename);
    }
}
