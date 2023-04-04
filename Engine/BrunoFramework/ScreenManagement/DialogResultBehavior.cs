
using Microsoft.Xaml.Behaviors;
using System;
using System.Windows;

namespace BrunoFramework
{
    public class DialogResultBehavior : Behavior<Window>
    {
        public static readonly DependencyProperty DialogResultProperty = DependencyProperty.Register(
            "DialogResult",
            typeof(bool?),
            typeof(DialogResultBehavior),
            new FrameworkPropertyMetadata(null, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault, OnDialogResultChanged));

        public bool? DialogResult
        {
            get { return (bool?)GetValue(DialogResultProperty); }
            set { SetValue(DialogResultProperty, value); }
        }

        protected override void OnAttached()
        {
            base.OnAttached();
            AssociatedObject.Closed += OnWindowClosed;
        }

        protected override void OnDetaching()
        {
            AssociatedObject.Closed -= OnWindowClosed;
            base.OnDetaching();
        }

        private void OnWindowClosed(object sender, EventArgs eventArgs)
        {
            var window = sender as Window;
            if (window != null)
            {
                DialogResult = window.DialogResult;
            }
        }

        private static void OnDialogResultChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            var behavior = (DialogResultBehavior)dependencyObject;
            var window = behavior.AssociatedObject;
            bool? dialogResult = (bool?)eventArgs.NewValue;
            if (window.IsLoaded && window.DialogResult != dialogResult)
            {
                window.DialogResult = dialogResult;
            }
        }
    }
}
