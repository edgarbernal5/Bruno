﻿using System;

namespace TrioWpfFramework.ServiceLocation
{
    /// <summary>
    /// Specifies when a service instance will be disposed.
    /// </summary>
    /// <remarks>
    /// The <see cref="DisposalPolicy"/> is only relevant for services that implement 
    /// <see cref="IDisposable"/>.
    /// </remarks>
    public enum DisposalPolicy
    {
        /// <summary>
        /// Specifies that the service instance will automatically be disposed when the service is
        /// removed from the <see cref="ServiceContainer"/> or when the
        /// <see cref="ServiceContainer"/> is disposed. (The service's lifetime is bound to the
        /// lifetime of the container.)
        /// </summary>
        Automatic,


        /// <summary>
        /// Specifies that the service instance is not automatically disposed when the
        /// <see cref="ServiceContainer"/> is disposed. (The service is owned by an external object,
        /// which is responsible for disposing the instance.)
        /// </summary>
        Manual,
    }
}
