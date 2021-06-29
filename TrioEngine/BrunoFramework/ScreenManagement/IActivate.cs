﻿using System;
using System.Threading;
using System.Threading.Tasks;

namespace BrunoFramework
{
    public interface IActivate
    {
        bool IsActive { get; }

        void Activate();

        event EventHandler<ActivationEventArgs> Activated;
    }

    public class ActivationEventArgs : EventArgs
    {
        public bool WasInitialized { get; set; }
    }
}