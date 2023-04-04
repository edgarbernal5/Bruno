
using System.Collections.Generic;

namespace BrunoFramework.Editor.Game
{
    public interface IWorldOutlineService
    {
        void Search(string keyword);
        void RemoveEntities(List<long> entities);
        void AddEmptyObject();

        WorldOutline WorldOutline { get; set; }
        WorldOutlineViewModel ViewModel { get; }
    }
}
