
using BrunoFramework.Editor.Game.Gizmos;
using System.Threading.Tasks;

namespace BrunoFramework.Editor.Units
{
    public interface ISceneService
    {
        Task ShowSnapSettingsAsync(GizmoSnappingConfig snappingConfig);
    }
}
