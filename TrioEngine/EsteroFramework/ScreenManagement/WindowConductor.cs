
using System;
using System.ComponentModel;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace EsteroFramework
{
    public class WindowConductor
    {
        private bool deactivatingFromView;
        private bool deactivateFromViewModel;
        private bool actuallyClosing;
        private readonly Window view;
        private readonly object viewModel;

        public WindowConductor(object viewModel, Window view)
        {
            this.viewModel = viewModel;
            this.view = view;
        }

        public void Initialize()
        {
            var activator = viewModel as IActivate;
            if (activator != null)
            {
                activator.Activate();
            }

            var deactivatable = viewModel as IDeactivate;
            if (deactivatable != null)
            {
                view.Closed += OnClosed;
                deactivatable.Deactivated += OnDeactivated;
            }

            var guard = viewModel as IGuardClose;
            if (guard != null)
            {
                view.Closing += OnClosing;
            }
        }

        private void OnDeactivated(object sender, DeactivationEventArgs eventArgs)
        {
            if (!eventArgs.WasClosed)
            {
                return;
            }

            ((IDeactivate)viewModel).Deactivated -= OnDeactivated;

            if (deactivatingFromView)
            {
                return;
            }

            deactivateFromViewModel = true;
            actuallyClosing = true;
            view.Close();
            actuallyClosing = false;
            deactivateFromViewModel = false;
        }

        private void OnClosed(object sender, EventArgs e)
        {
            view.Closed -= OnClosed;
            view.Closing -= OnClosing;

            if (deactivateFromViewModel)
            {
                return;
            }

            var deactivatable = viewModel as IDeactivate;

            deactivatingFromView = true;
            deactivatable.Deactivate(true);
            deactivatingFromView = false;
        }

        private async void OnClosing(object sender, CancelEventArgs eventArgs)
        {
            if (eventArgs.Cancel)
            {
                return;
            }

            if (actuallyClosing)
            {
                actuallyClosing = false;
                return;
            }

            var cachedDialogResult = view.DialogResult;

            eventArgs.Cancel = true;

            await Task.Yield();

            var guard = (IGuardClose)viewModel;
            var canClose = await guard.CanCloseAsync(CancellationToken.None);
            
            if (!canClose)
                return;

            actuallyClosing = true;

            if (cachedDialogResult == null)
            {
                view.Close();
            }
            else if (view.DialogResult != cachedDialogResult)
            {
                view.DialogResult = cachedDialogResult;
            }
        }
    }
}
