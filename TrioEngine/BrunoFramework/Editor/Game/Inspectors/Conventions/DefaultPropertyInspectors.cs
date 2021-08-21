using BrunoApi.Net.Maths;
using System.Collections.Generic;
using System.ComponentModel;

namespace BrunoFramework.Editor.Game.Inspectors.Conventions
{
    public static class DefaultPropertyInspectors
    {
        private static readonly List<PropertyEditorBuilder> _inspectorBuilders;

        public static List<PropertyEditorBuilder> InspectorBuilders
        {
            get { return _inspectorBuilders; }
        }

        static DefaultPropertyInspectors()
        {
            _inspectorBuilders = new List<PropertyEditorBuilder>
            {
                //new RangePropertyEditorBuilder(),
                //new EnumPropertyEditorBuilder(),

                //new StandardPropertyEditorBuilder<bool, CheckBoxEditorViewModel>(),
                //new StandardPropertyEditorBuilder<Color, ColorEditorViewModel>(),
                //new StandardPropertyEditorBuilder<double, TextBoxEditorViewModel<double>>(),
                //new StandardPropertyEditorBuilder<float, TextBoxEditorViewModel<float>>(),
                //new StandardPropertyEditorBuilder<int, TextBoxEditorViewModel<int>>(),
                //new StandardPropertyEditorBuilder<double?, TextBoxEditorViewModel<double?>>(),
                //new StandardPropertyEditorBuilder<float?, TextBoxEditorViewModel<float?>>(),
                //new StandardPropertyEditorBuilder<int?, TextBoxEditorViewModel<int?>>(),
                new StandardPropertyEditorBuilder<Vector3, Vector3EditorViewModel>(),
                new StandardPropertyEditorBuilder<Quaternion, QuaternionEditorViewModel>(),
                new StandardPropertyEditorBuilder<string, TextBoxEditorViewModel>(),
                //new StandardPropertyEditorBuilder<string, TextBoxEditorViewModel<string>>(),

                //new StandardPropertyEditorBuilder<BitmapSource, BitmapSourceEditorViewModel>()
            };
        }

        public static IInspectorEditor CreateEditor(PropertyDescriptor propertyDescriptor)
        {
            foreach (var inspectorBuilder in _inspectorBuilders)
            {
                if (inspectorBuilder.IsApplicable(propertyDescriptor))
                    return inspectorBuilder.BuildEditor(propertyDescriptor);
            }
            return null;
        }
    }
}
