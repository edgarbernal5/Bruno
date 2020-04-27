using Estero.Collections;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Controls
{
    /// <summary>
    /// Describes the properties which should be displayed in a <see cref="PropertyGrid"/>.
    /// </summary>
    public interface IPropertySource : INamedItem, INotifyPropertyChanged
    {
        /// <summary>
        /// Gets the type name of the property source.
        /// </summary>
        /// <value>The type name of the property source.</value>
        string TypeName { get; }


        /// <summary>
        /// Gets the properties.
        /// </summary>
        /// <value>A collection of properties.</value>
        ObservableCollection<IProperty> Properties { get; }
    }
}
