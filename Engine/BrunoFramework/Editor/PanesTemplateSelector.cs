
using AvalonDock.Layout;
using BrunoFramework.Editor.Game;
using BrunoFramework.Editor.Game.Properties;
using BrunoFramework.Editor.Units;
using System.Windows;
using System.Windows.Controls;

namespace BrunoFramework.Editor
{
    class PanesTemplateSelector : DataTemplateSelector
	{
		public PanesTemplateSelector()
		{

		}


		public DataTemplate SceneDocumentViewModelTemplate
		{
			get;
			set;
		}

        public DataTemplate WorldOutlineViewModelTemplate
        {
            get;
            set;
		}

		public DataTemplate PropertiesViewModelTemplate
		{
			get;
			set;
		}
		public DataTemplate ContentBrowserViewModelTemplate
		{
			get;
			set;
		}
		public DataTemplate ConsoleViewModelTemplate
		{
			get;
			set;
		}

		public override System.Windows.DataTemplate SelectTemplate(object item, System.Windows.DependencyObject container)
		{
			var itemAsLayoutContent = item as LayoutContent;

			if (item is SceneDocumentViewModel)
				return SceneDocumentViewModelTemplate;

			if (item is WorldOutlineViewModel)
				return WorldOutlineViewModelTemplate;

			if (item is PropertiesViewModel)
				return PropertiesViewModelTemplate;

			if (item is ContentBrowserViewModel)
				return ContentBrowserViewModelTemplate;

			if (item is ConsoleViewModel)
				return ConsoleViewModelTemplate;

			return base.SelectTemplate(item, container);
		}
	}
}
