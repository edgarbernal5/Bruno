using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.DataAnnotations;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using TrioWpfFramework.Windows;

namespace TrioWpfFramework.Windows
{
    public class ValidatableObservableObject : ObservableObject, INotifyDataErrorInfo
    {
        private const string HasErrorsPropertyName = "HasErrors";

        protected ErrorsContainer ErrorsContainer { get; }

        public bool HasErrors
        {
            get { return ErrorsContainer.HasErrors; }
        }

        /// <summary>
        /// Occurs when the validation errors have changed for a property or for the entire object.
        /// </summary>
        public event EventHandler<DataErrorsChangedEventArgs> ErrorsChanged;

        public ValidatableObservableObject()
        {
            ErrorsContainer = new ErrorsContainer(propertyName =>
            {
                OnErrorsChanged(new DataErrorsChangedEventArgs(propertyName));
                OnPropertyChanged(new PropertyChangedEventArgs(HasErrorsPropertyName));
            });
        }

        public IEnumerable GetErrors(string propertyName)
        {
            return ErrorsContainer.GetErrors(propertyName);
        }

        public Dictionary<string, List<string>> GetAllErrors()
        {
            return ErrorsContainer.GetAllErrors();
        }

        protected virtual void OnErrorsChanged(DataErrorsChangedEventArgs eventArgs)
        {
            ErrorsChanged?.Invoke(this, eventArgs);
        }

        protected override bool SetProperty<T>(ref T field, T value, [CallerMemberName] string propertyName = null)
        {
            bool hasPropertyChanged = base.SetProperty(ref field, value, propertyName);
            if (hasPropertyChanged && !string.IsNullOrEmpty(propertyName))
                ValidateProperty(propertyName);

            return hasPropertyChanged;
        }

        public bool ValidateProperty(string propertyName)
        {
            if (string.IsNullOrEmpty(propertyName))
                throw new ArgumentNullException(nameof(propertyName));

            var propertyInfo = GetType().GetRuntimeProperty(propertyName);
            if (propertyInfo == null)
                throw new ArgumentException("Invalid property name", propertyName);

            if (!propertyInfo.GetCustomAttributes(typeof(ValidationAttribute)).Any())
                return true;

            var errors = new List<string>();
            bool isValid = TryValidateProperty(propertyInfo, errors);
            ErrorsContainer.SetErrors(propertyInfo.Name, errors);
            return isValid;
        }

        public bool ValidateProperties()
        {
            // Get all the properties decorated with the ValidationAttribute.
            var propertiesToValidate = GetType().GetRuntimeProperties()
                                                .Where(c => c.GetCustomAttributes(typeof(ValidationAttribute)).Any());

            bool isValid = true;
            var errors = new List<string>();
            foreach (var propertyInfo in propertiesToValidate)
            {
                errors.Clear();
                isValid &= TryValidateProperty(propertyInfo, errors);
                ErrorsContainer.SetErrors(propertyInfo.Name, errors);
            }

            return isValid;
        }

        private bool TryValidateProperty(PropertyInfo propertyInfo, List<string> propertyErrors)
        {
            Debug.Assert(propertyInfo != null);
            Debug.Assert(propertyErrors != null);

            var validationResults = new List<ValidationResult>();
            var validationContext = new ValidationContext(this) { MemberName = propertyInfo.Name };
            var propertyValue = propertyInfo.GetValue(this);
            bool isValid = Validator.TryValidateProperty(propertyValue, validationContext, validationResults);
            foreach (var validationResult in validationResults)
                propertyErrors.Add(validationResult.ErrorMessage);

            return isValid;
        }
    }
}
