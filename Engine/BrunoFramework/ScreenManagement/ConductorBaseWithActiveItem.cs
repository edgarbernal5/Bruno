
using System.Threading;
using System.Threading.Tasks;

namespace BrunoFramework
{
    public abstract class ConductorBaseWithActiveItem<T> : ConductorBase<T>, IHaveActiveItem<T> where T : class
    {
        public T ActiveItem
        {
            get => m_activeItem;
            set => ActivateItemAsync(value, CancellationToken.None);
        }
        private T m_activeItem;

        protected virtual async Task ChangeActiveItemAsync(T newItem, bool closePrevious, CancellationToken cancellationToken)
        {
            await ScreenHelper.TryDeactivateAsync(m_activeItem, closePrevious, cancellationToken);

            newItem = EnsureItem(newItem);

            m_activeItem = newItem;
            NotifyOfPropertyChange(nameof(ActiveItem));

            if (IsActive)
            {
                await ScreenHelper.TryActivateAsync(newItem, cancellationToken);
            }
        }

        protected Task ChangeActiveItemAsync(T newItem, bool closePrevious) => ChangeActiveItemAsync(newItem, closePrevious, default);
    }

    public interface IHaveActiveItem<T> where T : class
    {
        T ActiveItem { get; set; }
    }
}
