using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Framework
{
    /// <summary>
    /// Provides helper methods for working with parallel tasks.
    /// </summary>
    public static class TaskHelper
    {
        // TODO: Cache reusable Task instances. Task.FromResult(false), Task.FromResult(true), etc.
        // For example, use ConcurrentDictionary<object, Task>.


        /// <summary>
        /// Creates a <see cref="Task"/> that has completed successfully.
        /// </summary>
        /// <returns>The successfully completed task.</returns>
        public static Task Completed()
        {
            return Task.FromResult<object>(null);
        }


        /// <summary>
        /// Creates a <see cref="Task{TResult}"/> that has completed successfully with the specified
        /// result.
        /// </summary>
        /// <typeparam name="T">The type of the result returned by the task.</typeparam>
        /// <param name="result">The result to store into the completed task.</param>
        /// <returns>The successfully completed task.</returns>
        public static Task<T> FromResult<T>(T result)
        {
            return Task.FromResult(result);
        }


        /// <summary>
        /// Takes an asynchronous task and does nothing. (Used to indicate that no 'await' is
        /// required and suppress warning CS4014.)
        /// </summary>
        /// <param name="task">The task that is not awaited.</param>
        public static void Forget(this Task task)
        {
        }
    }
}

