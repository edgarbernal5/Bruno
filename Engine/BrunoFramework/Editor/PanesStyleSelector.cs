
using System.Windows;
using System.Windows.Controls;

namespace BrunoFramework.Editor
{
    class PanesStyleSelector : StyleSelector
    {
        public Style EditorDockableTabStyle
        {
            get;
            set;
		}

		public override System.Windows.Style SelectStyle(object item, System.Windows.DependencyObject container)
		{
			if (item is EditorDockableTabViewModel)
				return EditorDockableTabStyle;

			return base.SelectStyle(item, container);
		}
	}
}
