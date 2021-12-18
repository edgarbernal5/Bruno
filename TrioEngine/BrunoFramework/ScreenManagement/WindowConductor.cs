
using System;
using System.ComponentModel;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace BrunoFramework
{
    public class WindowConductor
    {
        private bool m_deactivatingFromView;
        private bool m_deactivateFromViewModel;
        private bool m_actuallyClosing;
        private readonly Window m_view;
        private readonly object m_viewModel;

        public WindowConductor(object viewModel, Window view)
        {
            m_viewModel = viewModel;
            m_view = view;
        }

        public void Initialize()
        {
            var activator = m_viewModel as IActivate;
            if (activator != null)
            {
                activator.Activate();
            }

            var deactivatable = m_viewModel as IDeactivate;
            if (deactivatable != null)
            {
                m_view.Closed += OnClosed;
                deactivatable.Deactivated += OnDeactivated;
            }

            var guard = m_viewModel as IGuardClose;
            if (guard != null)
            {
                m_view.Closing += OnClosing;
            }
        }

        private void OnDeactivated(object sender, DeactivationEventArgs eventArgs)
        {
            if (!eventArgs.WasClosed)
            {
                return;
            }

            ((IDeactivate)m_viewModel).Deactivated -= OnDeactivated;

            if (m_deactivatingFromView)
            {
                return;
            }

            m_deactivateFromViewModel = true;
            m_actuallyClosing = true;
            m_view.Close();
            m_actuallyClosing = false;
            m_deactivateFromViewModel = false;
        }

        private void OnClosed(object sender, EventArgs e)
        {
            m_view.Closed -= OnClosed;
            m_view.Closing -= OnClosing;

            if (m_deactivateFromViewModel)
            {
                return;
            }

            var deactivatable = m_viewModel as IDeactivate;

            m_deactivatingFromView = true;
            deactivatable.Deactivate(true);
            m_deactivatingFromView = false;
        }

        private async void OnClosing(object sender, CancelEventArgs eventArgs)
        {
            if (eventArgs.Cancel)
            {
                return;
            }

            if (m_actuallyClosing)
            {
                m_actuallyClosing = false;
                return;
            }

            var cachedDialogResult = m_view.DialogResult;

            eventArgs.Cancel = true;

            await Task.Yield();

            var guard = (IGuardClose)m_viewModel;
            var canClose = await guard.CanCloseAsync(CancellationToken.None);
            
            if (!canClose)
                return;

            m_actuallyClosing = true;

            if (cachedDialogResult == null)
            {
                m_view.Close();
            }
            else if (m_view.DialogResult != cachedDialogResult)
            {
                m_view.DialogResult = cachedDialogResult;
            }
        }
    }
}
