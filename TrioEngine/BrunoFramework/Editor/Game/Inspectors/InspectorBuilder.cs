
using BrunoApi.Net.Maths;
using BrunoFramework.Editor.Game.Inspectors.Conventions;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Linq.Expressions;

namespace BrunoFramework.Editor.Game.Inspectors
{
    public class InspectorBuilder<TBuilder>
        where TBuilder : InspectorBuilder<TBuilder>
    {
        private readonly List<IInspector> _inspectors;

        protected List<IInspector> Inspectors
        {
            get { return _inspectors; }
        }

        public InspectorBuilder()
        {
            _inspectors = new List<IInspector>();
        }

        public TBuilder WithPoint3DEditor<T>(T instance, Expression<Func<T, Vector3>> propertyExpression)
        {
            return WithEditor<T, Vector3, Vector3EditorViewModel>(instance, propertyExpression);
        }

        public TBuilder WithEditor<T, TProperty, TEditor>(T instance, Expression<Func<T, TProperty>> propertyExpression)
            where TEditor : IEditor, new()
        {
            return WithEditor(instance, propertyExpression, new TEditor());
        }

        public TBuilder WithEditor<T, TProperty, TEditor>(T instance, Expression<Func<T, TProperty>> propertyExpression, TEditor editor)
            where TEditor : IEditor
        {
            var propertyName = ExpressionUtility.GetPropertyName(propertyExpression);
            editor.BoundPropertyDescriptor = BoundPropertyDescriptor.FromProperty(instance, propertyName);
            _inspectors.Add(editor);
            return (TBuilder)this;
        }

        public TBuilder WithObjectProperties(object instance, Func<PropertyDescriptor, bool> propertyFilter)
        {
            var properties = TypeDescriptor.GetProperties(instance)
                .Cast<PropertyDescriptor>()
                .Where(x => x.IsBrowsable && propertyFilter(x))
                .ToList();

            // If any properties are not in the default group, show all properties in collapsible groups.
            //if (properties.Any(x => !string.IsNullOrEmpty(x.Category) && x.Category != CategoryAttribute.Default.Category))
            //{
            //    foreach (var category in properties.GroupBy(x => x.Category))
            //    {
            //        var actualCategory = (string.IsNullOrEmpty(category.Key) || category.Key == CategoryAttribute.Default.Category)
            //            ? "Misc"
            //            : category.Key;

            //        var collapsibleGroupBuilder = new CollapsibleGroupBuilder();
            //        AddProperties(instance, category, collapsibleGroupBuilder.Inspectors);
            //        if (collapsibleGroupBuilder.Inspectors.Any())
            //            _inspectors.Add(collapsibleGroupBuilder.ToCollapsibleGroup(actualCategory));
            //    }
            //}
            //else // Otherwise, show properties in flat list.
            {
                AddProperties(instance, properties, _inspectors);
            }

            return (TBuilder)this;
        }

        public TBuilder WithObjectProperty(object instance, PropertyDescriptor property)
        {
            var editor = DefaultPropertyInspectors.CreateEditor(property);
            if (editor != null)
            {
                editor.BoundPropertyDescriptor = new BoundPropertyDescriptor(instance, property);
                _inspectors.Add(editor);
            }

            return (TBuilder)this;
        }

        private static void AddProperties(object instance, IEnumerable<PropertyDescriptor> properties, List<IInspector> inspectors)
        {
            foreach (var property in properties)
            {
                var editor = DefaultPropertyInspectors.CreateEditor(property);
                if (editor != null)
                {
                    editor.BoundPropertyDescriptor = new BoundPropertyDescriptor(instance, property);
                    inspectors.Add(editor);
                }
            }
        }
    }

    public class InspectableObjectBuilder : InspectorBuilder<InspectableObjectBuilder>
    {
        public InspectableObject ToInspectableObject()
        {
            return new InspectableObject(Inspectors);
        }
    }
}
