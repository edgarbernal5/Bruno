
using System.Collections.Generic;

namespace BrunoFramework.Editor.Game.Properties
{
    public interface IPropertiesObject
    {
        IEnumerable<IProperties> Inspectors { get; }
    }

    public class PropertiesObject : IPropertiesObject
    {
        public IEnumerable<IProperties> Inspectors { get; set; }

        public PropertiesObject(IEnumerable<IProperties> inspectors)
        {
            Inspectors = inspectors;
        }
    }
}
