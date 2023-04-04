
using System.Windows;
using System.Windows.Controls;

namespace BrunoFramework.Editor.Game.Properties
{
    public class PropertiesItemTemplateSelector : DataTemplateSelector
    {
        public DataTemplate LabelledTemplate { get; set; }
        public DataTemplate DefaultTemplate { get; set; }

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            if (item is ILabelledProperties)
                return LabelledTemplate;
            return DefaultTemplate;
        }
    }
}
