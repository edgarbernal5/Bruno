
using System.ComponentModel;

namespace BrunoFramework.Editor.Game.Inspectors.Conventions
{
    public class StandardPropertyEditorBuilder<T, TEditor> : PropertyEditorBuilder
        where TEditor : IInspectorEditor, new()
    {
        public override bool IsApplicable(PropertyDescriptor propertyDescriptor)
        {
            return propertyDescriptor.PropertyType == typeof(T);
        }

        public override IInspectorEditor BuildEditor(PropertyDescriptor propertyDescriptor)
        {
            return new TEditor();
        }
    }
}
