
using BrunoApi.Net.Graphics.Core;
using BrunoApi.Net.Maths;

namespace BrunoFramework.Graphics.Editor
{
    public interface IAxisGizmoRenderer
    {
        void SetColors(ColorRGBA8[] axisColors);
        void Render(RenderContext context);
        void SetWorld(Matrix gizmoWorld);
    }
}
