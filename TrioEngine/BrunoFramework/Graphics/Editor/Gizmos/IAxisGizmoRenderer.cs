
using BrunoApi.Net.Maths;

namespace BrunoFramework.Graphics.Editor
{
    public interface IAxisGizmoRenderer
    {
        void Render(RenderContext context);
        void SetWorld(Matrix gizmoWorld);
    }
}
