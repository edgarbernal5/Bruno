
using System.Collections.Generic;

namespace BrunoFramework.Editor.Game.Inspectors
{
    public interface IInspectableObject
    {
        IEnumerable<IInspector> Inspectors { get; }
    }

    public class InspectableObject : IInspectableObject
    {
        public IEnumerable<IInspector> Inspectors { get; set; }

        public InspectableObject(IEnumerable<IInspector> inspectors)
        {
            Inspectors = inspectors;
        }
    }
}
