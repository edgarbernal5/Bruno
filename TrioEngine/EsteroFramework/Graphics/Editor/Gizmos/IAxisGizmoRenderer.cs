
using TrioApi.Net.Maths;

namespace EsteroFramework.Graphics.Editor
{
    public interface IAxisGizmoRenderer
    {
        void Render(RenderContext context);
        void SetWorld(Matrix gizmoWorld);
    }
}
