
using System.Threading;
using System.Threading.Tasks;

namespace BrunoFramework
{
    public abstract class ConductorBase<T> : Screen, IConductor where T : class
    {
        public ICloseStrategy<T> CloseStrategy
        {
            get => m_closeStrategy;
            set => m_closeStrategy = value;
        }
        private ICloseStrategy<T> m_closeStrategy = new DefaultCloseStrategy<T>();

        public Task ActivateItemAsync(object item, CancellationToken cancellationToken = default)
        {
            return ActivateItemAsync((T)item, cancellationToken);
        }

        public abstract Task ActivateItemAsync(T item, CancellationToken cancellationToken = default);

        public Task DeactivateItemAsync(object item, bool close, CancellationToken cancellationToken = default)
        {
            return DeactivateItemAsync((T)item, close, cancellationToken);
        }

        public abstract Task DeactivateItemAsync(T item, bool close, CancellationToken cancellationToken = default);

        protected virtual T EnsureItem(T newItem)
        {
            var itemScreen = newItem as IScreen;
            if (itemScreen != null && itemScreen.Conductor != this)
            {
                itemScreen.Conductor = this;
            }

            return newItem;
        }
    }
}
