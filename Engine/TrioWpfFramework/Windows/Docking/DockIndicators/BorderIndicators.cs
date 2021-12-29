using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace TrioWpfFramework.Windows.Docking
{
    /// <summary>
    /// Appears over the <see cref="DockControl"/> when a <see cref="DockTabItem"/> is dragged and
    /// visualizes the areas where the user can dock the window.
    /// </summary>
    /// <remarks>
    /// <para>
    /// By default, the <see cref="BorderIndicators"/> shows dock indicators to dock the
    /// <see cref="DockTabItem"/> at the borders of the <see cref="DockControl"/>.
    /// </para>
    /// <para>
    /// See base class <see cref="DockIndicatorOverlay"/> for informations regarding styling.
    /// </para>
    /// </remarks>
    public class BorderIndicators : DockIndicatorOverlay
    {
        /// <summary>
        /// Initializes static members of the <see cref="BorderIndicators"/> class.
        /// </summary>
        static BorderIndicators()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(BorderIndicators), new FrameworkPropertyMetadata(typeof(BorderIndicators)));
        }


        /// <summary>
        /// Initializes a new instance of the <see cref="BorderIndicators"/> class.
        /// </summary>
        /// <param name="target">
        /// The target element over which the indicators should appear. (Typically the
        /// <see cref="DockControl"/>.)
        /// </param>
        public BorderIndicators(FrameworkElement target)
            : base(target)
        {
        }
    }
}
