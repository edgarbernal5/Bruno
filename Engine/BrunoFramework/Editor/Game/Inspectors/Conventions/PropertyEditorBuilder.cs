
using System.ComponentModel;

namespace BrunoFramework.Editor.Game.Inspectors.Conventions
{
    public abstract class PropertyEditorBuilder
    {
        public abstract bool IsApplicable(PropertyDescriptor propertyDescriptor);
        public abstract IInspectorEditor BuildEditor(PropertyDescriptor propertyDescriptor);
    }
}
