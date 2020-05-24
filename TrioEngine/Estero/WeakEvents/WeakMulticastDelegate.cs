using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Estero
{
    /// <summary>
    /// Represents a <see cref="MulticastDelegate"/> that stores the target objects as weak 
    /// references.
    /// </summary>
    /// <remarks>
    /// <strong>Important:</strong> In Silverlight, the targets of a 
    /// <see cref="WeakMulticastDelegate"/> need to be public methods (no private, protected or
    /// anonymous methods). This is necessary because of security restrictions in Silverlight.
    /// </remarks>
    public class WeakMulticastDelegate
    {
        private readonly List<InternalWeakDelegate> _delegates = new List<InternalWeakDelegate>();
        
        /// <summary>
        /// Gets the number of live delegates in the collection.
        /// </summary>
        /// <value>The number of live delegates in the collection.</value>
        public int Count
        {
            get
            {
                Purge();
                return _delegates.Count;
            }
        }


        /// <summary>
        /// Adds a new <see cref="Delegate"/> to the <see cref="WeakMulticastDelegate"/>.
        /// </summary>
        /// <param name="delegate">The new <see cref="Delegate"/>.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="delegate"/> is <see langword="null"/>.
        /// </exception>
        public void Add(Delegate @delegate)
        {
            if (@delegate == null)
                throw new ArgumentNullException("delegate");

            if (_delegates.Count == _delegates.Capacity)
                Purge();

            _delegates.Add(new InternalWeakDelegate(@delegate));
        }


        /// <summary>
        /// Removes a <see cref="Delegate"/> from the <see cref="WeakMulticastDelegate"/>.
        /// </summary>
        /// <param name="delegate">The <see cref="Delegate"/> to remove.</param>
        public void Remove(Delegate @delegate)
        {
            if (@delegate == null)
                return;

            for (int i = _delegates.Count - 1; i >= 0; i--)
            {
                InternalWeakDelegate weakDelegate = _delegates[i];
                if (weakDelegate.TargetReference != null)
                {
                    // The delegate method is an object method.
                    object target = weakDelegate.TargetReference.Target;
                    if (target == null)
                    {
                        // Remove garbage collected entry.
                        _delegates.RemoveAt(i);
                    }
                    else if (target == @delegate.Target
                  && weakDelegate.MethodInfo.Equals(@delegate.Method))

                    {
                        // Remove matching entry.
                        _delegates.RemoveAt(i);
                        break;
                    }
                }
                else
                {
                    // The delegate method is a class method.
                    if (@delegate.Target == null
              && weakDelegate.MethodInfo.Equals(@delegate.Method))

                    {
                        // Remove matching entry.
                        _delegates.RemoveAt(i);
                        break;
                    }
                }
            }
        }


        /// <summary>
        /// Invokes the stored <see cref="Delegate"/>s with the given arguments.
        /// </summary>
        /// <param name="args">The arguments.</param>
        public void Invoke(params object[] args)
        {
            Purge();
            foreach (InternalWeakDelegate weakDelegate in _delegates.ToArray())
                weakDelegate.Invoke(args);
        }


        /// <summary>
        /// Purges the garbage-collected delegates.
        /// </summary>
        private void Purge()
        {
            // .NET CF does not support:
            //   _delegates.RemoveAll(d => !d.IsAlive);
            //
            // Therefore we have to write our own loop:
            for (int i = _delegates.Count - 1; i >= 0; i--)
            {
                var @delegate = _delegates[i];
                if (!@delegate.IsAlive)
                    _delegates.RemoveAt(i);
            }
        }
    }
}
