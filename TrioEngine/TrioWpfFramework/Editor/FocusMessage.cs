﻿using System;

namespace TrioWpfFramework.Editor
{
    /// <summary>
    /// Represents the message that is broadcast over the <see cref="IMessageBus"/> when a UI
    /// element should be focused.
    /// </summary>
    internal class FocusMessage
    {
        /// <summary>
        /// Gets the data context of the UI element which should receive the focus.
        /// </summary>
        /// <value>The data context of the UI element which should receive the focus.</value>
        public object DataContext { get; }


        /// <summary>
        /// Initializes a new instance of the <see cref="FocusMessage"/> class.
        /// </summary>
        /// <param name="dataContext">
        /// The data context of the UI element which should receive the focus.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="dataContext"/> is <see langword="null"/>.
        /// </exception>
        public FocusMessage(object dataContext)
        {
            if (dataContext == null)
                throw new ArgumentNullException(nameof(dataContext));

            DataContext = dataContext;
        }
    }
}
