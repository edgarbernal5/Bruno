
using System.ComponentModel;

namespace BrunoFramework.Editor.Game.Properties.Conventions
{
    public abstract class PropertyEditorBuilder
    {
        public abstract bool IsApplicable(PropertyDescriptor propertyDescriptor);
        public abstract IPropertiesEditor BuildEditor(PropertyDescriptor propertyDescriptor);
    }
}
