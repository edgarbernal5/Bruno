﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Docking
{
    public interface IDockPane : IDockElement
    {
        bool IsVisible { get; set; }
    }
}
