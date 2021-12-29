using Estero.Collections;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Controls
{
    /// <summary>
    /// Describes a single property shown in a <see cref="PropertyGrid"/>.
    /// </summary>
    public interface IProperty : INamedItem, INotifyPropertyChanged
    {
        /// <summary>
        /// Gets the category.
        /// </summary>
        /// <value>The category.</value>
        string Category { get; }


        /// <summary>
        /// Gets the description.
        /// </summary>
        /// <value>The description.</value>
        string Description { get; }


        /// <summary>
        /// Gets a value indicating whether this instance is read only.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if this instance is read only; otherwise,
        /// <see langword="false"/>.
        /// </value>
        bool IsReadOnly { get; }


        /// <summary>
        /// Gets the type of the property.
        /// </summary>
        /// <value>The type of the property.</value>
        Type PropertyType { get; }


        /// <summary>
        /// Gets the value.
        /// </summary>
        /// <value>The value.</value>
        object Value { get; }


        /// <summary>
        /// Gets the resource key of the data template which is shown in the property grid.
        /// </summary>
        /// <value>
        /// The resource key of the data template which is shown in the property grid.
        /// </value>
        /// <remarks>
        /// If this value is <see langword="null"/>, a default template is used depending on the
        /// <see cref="PropertyType"/>. This property only needs to be set if a different data
        /// template than the default template should be used.
        /// </remarks>
        object DataTemplateKey { get; }


        /// <summary>
        /// Gets a value indicating whether this property can be <see cref="Reset"/>.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if this property can be reset; otherwise,
        /// <see langword="false"/>.
        /// </value>
        bool CanReset { get; }


        /// <summary>
        /// Resets this property to its default value.
        /// </summary>
        /// <remarks>
        /// This method does nothing if <see cref="CanReset"/> is <see langword="false"/>.
        /// </remarks>
        void Reset();
    }
}