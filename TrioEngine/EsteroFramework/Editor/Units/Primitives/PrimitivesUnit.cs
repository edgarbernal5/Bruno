using EsteroFramework.Editor.Game.Gizmos;
using EsteroFramework.Graphics;
using EsteroFramework.Graphics.Editor;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EsteroFramework.Editor.Units
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

            var gizmoService = new GizmoService(graphicsService);
            Editor.Services.RegisterInstance(typeof(IGizmoService), null, gizmoService);
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
