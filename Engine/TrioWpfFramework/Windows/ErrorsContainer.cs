using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace TrioWpfFramework.Windows
{
    public class ErrorsContainer
    {
        private readonly Dictionary<string, List<string>> _errorsPerProperty = new Dictionary<string, List<string>>();
        private readonly Action<string> _errorsChanged;

        public bool HasErrors
        {
            get
            {
                if (!_hasErrors.HasValue)
                {
                    _hasErrors = false;
                    foreach (var errors in _errorsPerProperty.Values)
                    {
                        if (errors.Count > 0)
                        {
                            _hasErrors = true;
                            break;
                        }
                    }
                }

                return _hasErrors.Value;
            }
        }
        private bool? _hasErrors;

        public ErrorsContainer(Action<string> errorsChanged)
        {
            if (errorsChanged == null)
                throw new ArgumentNullException(nameof(errorsChanged));

            _errorsChanged = errorsChanged;
        }

        public IEnumerable GetErrors(string propertyName)
        {
            if (propertyName == null)
                propertyName = string.Empty;

            List<string> errors;
            if (_errorsPerProperty.TryGetValue(propertyName, out errors))
                return errors;

            return Enumerable.Empty<string>();
        }
        public Dictionary<string, List<string>> GetAllErrors()
        {
            return _errorsPerProperty;
        }

        public void AddError(string propertyName, string error)
        {
            if (propertyName == null)
                propertyName = string.Empty;

            List<string> errors;
            if (!_errorsPerProperty.TryGetValue(propertyName, out errors))
            {
                errors = new List<string>();
                _errorsPerProperty.Add(propertyName, errors);
            }

            if (errors.Contains(error))
                return;

            errors.Add(error);
            _hasErrors = true;
            _errorsChanged(propertyName);
        }

        public void RemoveError(string propertyName, string validationError)
        {
            if (propertyName == null)
                propertyName = string.Empty;

            List<string> errors;
            if (!_errorsPerProperty.TryGetValue(propertyName, out errors))
                return;

            if (!errors.Contains(validationError))
                return;

            errors.Remove(validationError);
            _hasErrors = null;
            _errorsChanged(propertyName);
        }

        public void ClearErrors(string propertyName)
        {
            if (propertyName == null)
                propertyName = string.Empty;

            List<string> errors;
            if (!_errorsPerProperty.TryGetValue(propertyName, out errors))
                return;

            if (errors.Count == 0)
                return;

            errors.Clear();
            _hasErrors = null;
            _errorsChanged(propertyName);
        }

        public void SetErrors(string propertyName, IEnumerable<string> validationErrors)
        {
            if (propertyName == null)
                propertyName = string.Empty;

            if (validationErrors == null || !validationErrors.Any())
            {
                ClearErrors(propertyName);
            }
            else
            {
                List<string> errors;
                if (!_errorsPerProperty.TryGetValue(propertyName, out errors))
                {
                    errors = new List<string>();
                    _errorsPerProperty.Add(propertyName, errors);
                }

                errors.Clear();
                errors.AddRange(validationErrors);
                _hasErrors = true;
                _errorsChanged(propertyName);
            }
        }
    }
}
