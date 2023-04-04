using BrunoFramework.Editor.Game.Gizmos;
using BrunoFramework.Graphics;

using BrunoFramework.Graphics.Editor;

namespace BrunoFramework.Editor.Units
{
    public class PrimitivesUnit : EditorUnit
    {
        public PrimitivesUnit()
        {
        }

        protected override void OnInitialize()
        {
            var graphicsService = Editor.Services.GetInstance<IGraphicsService>();

            var editorPrimitivesService = new EditorPrimitivesService(graphicsService);
            Editor.Services.RegisterInstance(typeof(IEditorPrimitivesService), null, editorPrimitivesService);
        }

        protected override void OnStartup()
        {
            var editorPrimitivesService = Editor.Services.GetInstance<IEditorPrimitivesService>();
            editorPrimitivesService.InitializeMeshes();
        }

        protected override void OnUninitialize()
        {
            Editor.Services.UnregisterHandler(typeof(IEditorPrimitivesService));
        }

        protected override void OnShutdown()
        {

        }
    }
}
