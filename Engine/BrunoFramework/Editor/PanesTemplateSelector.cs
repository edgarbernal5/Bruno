
using AvalonDock.Layout;
using BrunoFramework.Editor.Game;
using BrunoFramework.Editor.Game.Inspectors;
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

		public DataTemplate InspectorViewModelTemplate
		{
			get;
			set;
		}
		public DataTemplate ContentBrowserViewModelTemplate
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

			if (item is InspectorViewModel)
				return InspectorViewModelTemplate;

			if (item is ContentBrowserViewModel)
				return ContentBrowserViewModelTemplate;

			return base.SelectTemplate(item, container);
		}
	}
}
