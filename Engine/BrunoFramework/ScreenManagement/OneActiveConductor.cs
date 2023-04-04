
using Bruno.Linq;
using BrunoWindows;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace BrunoFramework
{
    public class OneActiveConductor<T> : ConductorBaseWithActiveItem<T> where T : class
    {
        public BindableCollection<T> Items => m_items;
        private readonly BindableCollection<T> m_items = new BindableCollection<T>();

        public OneActiveConductor()
        {
            m_items.CollectionChanged += (s, e) =>
            {
                switch (e.Action)
                {
                    case NotifyCollectionChangedAction.Add:
                        e.NewItems.OfType<IScreen>().ForEach(x => x.Conductor = this);
                        break;
                    case NotifyCollectionChangedAction.Remove:
                        e.OldItems.OfType<IScreen>().ForEach(x => x.Conductor = null);
                        break;
                    case NotifyCollectionChangedAction.Replace:
                        e.NewItems.OfType<IScreen>().ForEach(x => x.Conductor = this);
                        e.OldItems.OfType<IScreen>().ForEach(x => x.Conductor = null);
                        break;
                    case NotifyCollectionChangedAction.Reset:
                        m_items.OfType<IScreen>().ForEach(x => x.Conductor = this);
                        break;
                }
            };
        }

        public override async Task ActivateItemAsync(T item, CancellationToken cancellationToken = default)
        {
            if (item != null && item.Equals(ActiveItem))
            {
                if (IsActive)
                {
                    await ScreenHelper.TryActivateAsync(item, cancellationToken);
                }
                return;
            }

            await ChangeActiveItemAsync(item, false, cancellationToken);
        }

        public override async Task DeactivateItemAsync(T item, bool close, CancellationToken cancellationToken = default)
        {
            if (item == null)
                return;

            if (!close)
            {
                await ScreenHelper.TryDeactivateAsync(item, false, cancellationToken);
            }
            else
            {
                var closeResult = await CloseStrategy.ExecuteAsync(new[] { item }, CancellationToken.None);

                if (closeResult)
                {
                    await CloseItemCoreAsync(item, cancellationToken);
                }
            }
        }

        private async Task CloseItemCoreAsync(T item, CancellationToken cancellationToken = default)
        {
            if (item.Equals(ActiveItem))
            {
                var index = m_items.IndexOf(item);
                var next = DetermineNextItemToActivate(m_items, index);

                await ChangeActiveItemAsync(next, true);
            }
            else
            {
                await ScreenHelper.TryDeactivateAsync(item, true, cancellationToken);
            }

            m_items.Remove(item);
        }

        protected virtual T DetermineNextItemToActivate(IList<T> list, int lastIndex)
        {
            var toRemoveAt = lastIndex - 1;

            if (toRemoveAt == -1 && list.Count > 1)
            {
                return list[1];
            }

            if (toRemoveAt > -1 && toRemoveAt < list.Count - 1)
            {
                return list[toRemoveAt];
            }

            return default(T);
        }

        public override async Task<bool> CanCloseAsync(CancellationToken cancellationToken = default)
        {
            var closeResult = await CloseStrategy.ExecuteAsync(m_items.ToList(), cancellationToken);
            return closeResult;
        }

        protected override Task OnActivateAsync(CancellationToken cancellationToken)
        {
            return ScreenHelper.TryActivateAsync(ActiveItem, cancellationToken);
        }

        protected override async Task OnDeactivateAsync(bool close, CancellationToken cancellationToken)
        {
            if (close)
            {
                foreach (var deactivate in m_items.OfType<IDeactivate>())
                {
                    await deactivate.DeactivateAsync(true, cancellationToken);
                }

                m_items.Clear();
            }
            else
            {
                await ScreenHelper.TryDeactivateAsync(ActiveItem, false, cancellationToken);
            }
        }

        protected override T EnsureItem(T newItem)
        {
            if (newItem == null)
            {
                newItem = DetermineNextItemToActivate(m_items, ActiveItem != null ? m_items.IndexOf(ActiveItem) : 0);
            }
            else
            {
                var index = m_items.IndexOf(newItem);

                if (index == -1)
                {
                    m_items.Add(newItem);
                }
                else
                {
                    newItem = m_items[index];
                }
            }

            return base.EnsureItem(newItem);
        }
    }
}
