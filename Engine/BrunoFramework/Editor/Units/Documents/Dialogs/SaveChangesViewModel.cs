
using BrunoWindows;
using System.Collections.Generic;

namespace BrunoFramework.Editor.Units
{
    public class SaveChangesViewModel : Dialog
    {
        public IEnumerable<Document> ModifiedDocuments
        {
            get => m_modifiedDocuments;
            set
            {
                m_modifiedDocuments = value;
                NotifyOfPropertyChange();
            }
        }
        private IEnumerable<Document> m_modifiedDocuments;

        public SaveChangesDialogResult SaveChangesDialogResult
        {
            get => m_saveChangesDialogResult;
            set
            {
                m_saveChangesDialogResult = value;
                NotifyOfPropertyChange();
            }
        }
        private SaveChangesDialogResult m_saveChangesDialogResult = SaveChangesDialogResult.Cancel;

        public DelegateCommand YesCommand { get; private set; }

        public DelegateCommand NoCommand { get; private set; }

        public DelegateCommand CancelCommand { get; private set; }

        public SaveChangesViewModel()
        {
            if (WindowsPlatform.InDesignMode) return;

            YesCommand = new DelegateCommand(DoYes);
            NoCommand = new DelegateCommand(DoNo);
            CancelCommand = new DelegateCommand(DoCancel);
        }

        private void DoYes()
        {
            SaveChangesDialogResult = SaveChangesDialogResult.SaveAndClose;
            DialogResult = true;
        }

        private void DoNo()
        {
            SaveChangesDialogResult = SaveChangesDialogResult.CloseWithoutSaving;
            DialogResult = true;
        }

        private void DoCancel()
        {
            SaveChangesDialogResult = SaveChangesDialogResult.Cancel;
            DialogResult = false;
        }
    }
}
