
using System.Collections.Generic;

namespace BrunoFramework.Editor.Game
{
    public interface IWorldOutlineHandler : IWorldOutlineSelectionHandler, IWorldOutlineAddObjectHandler
    {
        void DeleteObjects(List<long> entities);
    }

    public interface IWorldOutlineSelectionHandler
    {
        void SelectionChanged();
    }

    public interface IWorldOutlineAddObjectHandler
    {
        void AddEmptyObject(string name);
    }
}
