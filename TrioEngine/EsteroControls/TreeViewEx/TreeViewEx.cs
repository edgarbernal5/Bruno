using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace EsteroControls
{
    [TemplatePart(Name = PART_ScrollViewer, Type = typeof(ScrollViewer))]
    public class TreeViewEx : ItemsControl
    {
        private const string PART_ScrollViewer = nameof(PART_ScrollViewer);

        private Window _window;
        private ScrollViewer _scrollViewer;
        private TreeViewItemEx _shiftSelectionAnchor;
        private TreeViewItemEx _lastFocusedContainer;

    }
}
