
using System.Threading;
using System.Threading.Tasks;

namespace BrunoFramework.Editor.Units
{
    public abstract class DocumentViewModel : EditorDockableTabViewModel
    {
        public Document Document { get; }

        protected DocumentViewModel(Document document)
        {
            Document = document;
            DisplayName = document.GetDisplayName();
        }

        protected override void OnActivate()
        {
            Document.RegisterViewModel(this);

            base.OnActivate();
        }

        protected override void OnDeactivate(bool close)
        {
            if (close)
            {
                Document.UnregisterViewModel(this);

                if (!Document.IsDisposed)
                {
                    Document.DocumentService.Close(Document, true);
                }
            }
            base.OnDeactivate(close);
        }

        public override Task<bool> CanCloseAsync(CancellationToken cancellationToken = default)
        {
            if (Document.IsDisposed)
            {
                return Task.FromResult(true);
            }

            bool canClose = Document.DocumentService.CanClose(Document);
            return Task.FromResult(canClose);
        }
    }
}
