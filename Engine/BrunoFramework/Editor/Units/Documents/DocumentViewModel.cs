
using System;
using System.ComponentModel;
using System.Threading;
using System.Threading.Tasks;

namespace BrunoFramework.Editor.Units
{
    public abstract class DocumentViewModel : EditorDockTabViewModel
    {
        public Document Document { get; }

        protected DocumentViewModel(Document document)
        {
            Document = document;
            DisplayName = document.GetDisplayName();
        }

        protected override Task OnInitializeAsync(CancellationToken cancellationToken)
        {
            Document.RecordViewModel(this);
            Document.PropertyChanged += OnDocumentPropertyChanged;

            return base.OnInitializeAsync(cancellationToken);
        }

        private void OnDocumentPropertyChanged(object sender, PropertyChangedEventArgs eventArgs)
        {
            if (eventArgs.PropertyName == nameof(Document.IsModified) ||
                eventArgs.PropertyName == nameof(Document.IsUntitled) ||
                eventArgs.PropertyName == nameof(Document.FilePath))
            {
                DisplayName = Document.GetDisplayName();
            }
        }

        protected override Task OnDeactivateAsync(bool close, CancellationToken cancellationToken)
        {
            if (close)
            {
                Document.PropertyChanged -= OnDocumentPropertyChanged;

                if (!Document.IsDisposed)
                {
                    Document.DocumentService.Close(Document, true);
                }
                Document.UnrecordViewModel(this);
            }
            return base.OnDeactivateAsync(close, cancellationToken);
        }

        public override Task<bool> CanCloseAsync(CancellationToken cancellationToken = default)
        {
            if (Document.IsDisposed)
            {
                return Task.FromResult(true);
            }

            bool canClose = Document.DocumentService.ShowSaveChangesDialog(Document);
            return Task.FromResult(canClose);
        }
    }
}
