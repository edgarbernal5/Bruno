
using System;
using System.Collections.Generic;

namespace EsteroFramework.Editor.Units
{
    public abstract class ProjectFileFactory
    {
        public List<ProjectFileType> SupportedFileTypes { get; protected set; }

        protected ProjectFileFactory()
        {
            SupportedFileTypes = new List<ProjectFileType>();
        }

        public ProjectFile Create(ProjectFileType projectFileType)
        {
            if (!SupportedFileTypes.Exists(fileType => fileType.Name == projectFileType.Name))
                throw new InvalidOperationException();

            return OnCreate(projectFileType);
        }

        protected abstract ProjectFile OnCreate(ProjectFileType projectFileType);
    }
}
