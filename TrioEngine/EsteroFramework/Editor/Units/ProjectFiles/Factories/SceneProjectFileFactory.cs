
using EsteroFramework.Editor.Game;
using System;

namespace EsteroFramework.Editor.Units
{
    public class SceneProjectFileFactory : ProjectFileFactory
    {
        private IEditorService Editor { get; }

        public SceneProjectFileFactory(IEditorService editor) 
            : base()
        {
            Editor = editor;

            SupportedFileTypes.Add(new ProjectFileType("Scene", new string[] { ".x", ".fbx" }, this) );
        }

        protected override ProjectFile OnCreate(ProjectFileType projectFileType)
        {
            return new SceneProjectFile(Editor);
        }
    }
}
