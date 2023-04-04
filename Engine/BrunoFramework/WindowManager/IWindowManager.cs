using System;
using System.Threading.Tasks;

namespace BrunoFramework
{
    public interface IWindowManager
    {
        Task<bool?> ShowDialog(object viewModel, object context = null);

        Task ShowWindow(object viewModel, object context = null, bool asChildWindow = true);
    }
}
