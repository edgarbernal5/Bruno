﻿using BrunoWindows;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BrunoFramework.Editor
{
    public sealed class MenuSeparatorViewModel : MenuItemViewModel
    {
        public MenuSeparatorViewModel(CommandSeparator commandSeparator)
            : base(commandSeparator)
        {
        }

    }
}
