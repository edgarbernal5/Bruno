
using BrunoFramework.Editor.Game;
using System;

namespace BrunoFramework.Editor.Units
{
    public class SceneDocumentFactory : DocumentFactory
    {
        private IEditorService Editor { get; }

        public SceneDocumentFactory(IEditorService editor)
            : base()
        {
            Editor = editor;

            SupportedFileTypes.Add(new DocumentType("Scene", new string[] { ".x", ".fbx", ".obj" }, this) );
        }

        protected override Document OnCreate(DocumentType documentFileType)
        {
            return new SceneDocument(Editor, documentFileType);
        }
    }
}
