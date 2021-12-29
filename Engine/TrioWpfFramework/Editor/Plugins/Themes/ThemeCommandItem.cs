
using System;
using System.Windows.Input;
using TrioWpfFramework.Windows;
using TrioWpfFramework.Windows.Framework;
using static System.FormattableString;


namespace TrioWpfFramework.Editor.Themes
{
    /// <summary>
    /// Creates a menu or toolbar item for switching the UI theme.
    /// </summary>
    internal class ThemeCommandItem : ObservableObject, ICommandItem
    {
        private readonly ThemePlugin _themeExtension;
        private MenuItemViewModel _menuItemViewModel;
        private ToolBarComboBoxViewModel _toolBarComboBoxViewModel;

        public string Name
        {
            get { return "Theme"; }
        }

        public bool AlwaysShowText { get { return false; } }


        public string Category
        {
            get { return CommandCategories.Tools; }
        }

        public ICommand Command { get { return null; } }

        public object CommandParameter { get { return null; } }

        public object Icon { get { return null; } }

        public InputGestureCollection InputGestures { get { return null; } }

        public bool IsCheckable { get { return false; } }


        public bool IsChecked { get { return false; } }


        public string Text { get { return "Theme"; } }


        public string ToolTip
        {
            get { return "Change user interface theme."; }
        }

        public bool IsVisible
        {
            get { return _isVisible; }
            set { SetProperty(ref _isVisible, value); }
        }
        private bool _isVisible = true;

        /// <summary>
        /// Initializes a new instance of the <see cref="ThemeCommandItem"/> class.
        /// </summary>
        /// <param name="themeExtension">The theme extension.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="themeExtension"/> is <see langword="null"/>.
        /// </exception>
        public ThemeCommandItem(ThemePlugin themeExtension)
        {
            if (themeExtension == null)
                throw new ArgumentNullException(nameof(themeExtension));

            _themeExtension = themeExtension;
            _themeExtension.ThemeChanged += OnThemeChanged;
        }

        public MenuItemViewModel CreateMenuItem()
        {
            if (_menuItemViewModel == null)
            {
                _menuItemViewModel = new MenuItemViewModel(this) { Submenu = new MenuItemViewModelCollection() };

                var changeThemeCommand = new DelegateCommand<string>(ChangeTheme);

                foreach (var theme in _themeExtension.Themes)
                {
                    _menuItemViewModel.Submenu.Add(
                        new MenuItemViewModel(
                            new DelegateCommandItem("ChangeThemeTo" + theme, changeThemeCommand)
                            {
                                Category = Category,
                                CommandParameter = theme,
                                IsCheckable = true,
                                IsChecked = _themeExtension.Theme == theme,
                                Text = theme,
                                ToolTip = Invariant($"Change user interface theme to '{theme}'"),
                            }));
                }
            }

            return _menuItemViewModel;
        }


        public ToolBarItemViewModel CreateToolBarItem()
        {
            if (_toolBarComboBoxViewModel == null)
            {
                _toolBarComboBoxViewModel = new ToolBarComboBoxViewModel(this)
                {
                    Width = 75,
                    Items = _themeExtension.Themes,
                    SelectedItem = _themeExtension.Theme
                };

                _toolBarComboBoxViewModel.SelectedItemChanged += (s, e) => ChangeTheme((string)_toolBarComboBoxViewModel.SelectedItem);
            }

            return _toolBarComboBoxViewModel;
        }


        private void ChangeTheme(string theme)
        {
            _themeExtension.Theme = theme;
        }


        private void OnThemeChanged(object sender, EventArgs eventArgs)
        {
            UpdateMenuItem();
            UpdateToolBarItem();
        }


        private void UpdateMenuItem()
        {
            if (_menuItemViewModel != null)
            {
                foreach (var menuItem in _menuItemViewModel.Submenu)
                {
                    var commandItem = (DelegateCommandItem)menuItem.CommandItem;
                    commandItem.IsChecked = _themeExtension.Theme == (string)commandItem.CommandParameter;
                }
            }
        }


        private void UpdateToolBarItem()
        {
            if (_toolBarComboBoxViewModel != null)
                _toolBarComboBoxViewModel.SelectedItem = _themeExtension.Theme;
        }
    }
}
