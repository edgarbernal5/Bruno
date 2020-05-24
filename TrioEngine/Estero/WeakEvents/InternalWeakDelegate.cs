
using System;
using System.Diagnostics;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace Estero
{
    /// <summary>
    /// Stores <see cref="System.Delegate"/> that contains a weak reference to the target.
    /// </summary>
    internal struct InternalWeakDelegate
    {
        /// <summary>
        /// The weak reference of the target object, or <see langword="null"/> if the delegate method is
        /// a static method.
        /// </summary>
        public readonly WeakReference TargetReference;


        /// <summary>
        /// The metadata of the delegate method.
        /// </summary>
        public readonly MethodInfo MethodInfo;


        /// <summary>
        /// The type of the delegate.
        /// </summary>
        public readonly Type DelegateType;


        /// <summary>
        /// Gets the <see cref="System.Delegate"/> stored by the current 
        /// <see cref="InternalWeakDelegate"/> object.
        /// </summary>
        /// <value>
        /// <see langword="null"/> if the object referenced by the current 
        /// <see cref="InternalWeakDelegate"/> object has been garbage collected; otherwise, a 
        /// reference to the <see cref="System.Delegate"/>.
        /// </value>
        public Delegate Delegate
        {
            get
            {
                if (TargetReference == null)
                    return Delegate.CreateDelegate(DelegateType, null, MethodInfo);

                object target = TargetReference.Target;
                if (target != null)
                    return Delegate.CreateDelegate(DelegateType, target, MethodInfo);

                return null;
            }
        }


        /// <summary>
        /// Gets a value indicating whether the <see cref="System.Delegate"/> referenced by this 
        /// <see cref="InternalWeakDelegate"/> has been garbage collected.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if the <see cref="System.Delegate"/> referenced by the current 
        /// <see cref="InternalWeakDelegate"/> has not been garbage collected and is still 
        /// accessible; otherwise, <see langword="false"/>.
        /// </value>
        /// <remarks>
        /// <para>
        /// <strong>Important:</strong> Because an object could potentially be reclaimed for garbage 
        /// collection immediately after the <see cref="IsAlive"/> property returns 
        /// <see langword="true"/>, using this property is not recommended unless you are testing only 
        /// for a <see langword="false"/> return value. 
        /// </para>
        /// <para>
        /// If the referenced <see cref="System.Delegate"/> is a static method, <see cref="IsAlive"/> 
        /// will always return <see langword="true"/>.
        /// </para>
        /// </remarks>
        public bool IsAlive
        {
            get
            {
                return
                  TargetReference == null      // Delegate is a static method.
                  || TargetReference.IsAlive;  // Delegate points to a live object.
            }
        }


        /// <summary>
        /// Initializes a new instance of the <see cref="InternalWeakDelegate"/> class.
        /// </summary>
        /// <param name="delegate">
        /// The original <see cref="System.Delegate"/> to create a weak reference for.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="delegate"/> is <see langword="null"/>.
        /// </exception>
        public InternalWeakDelegate(Delegate @delegate)
        {
            if (@delegate == null)
                throw new ArgumentNullException("delegate");

            object target = @delegate.Target;
            TargetReference = (target != null) ? new WeakReference(target) : null;
            MethodInfo = @delegate.Method;


            DelegateType = @delegate.GetType();

            Debug.Assert(
              (TargetReference != null && !MethodInfo.IsStatic)
              || (TargetReference == null && MethodInfo.IsStatic),
              "Sanity check.");

            CheckForClosure(MethodInfo);
            CheckForNonPublicEventHandler(target, MethodInfo);
        }


        //--------------------------------------------------------------
        #region Methods
        //--------------------------------------------------------------

        /// <summary>
        /// Invokes the stored <see cref="System.Delegate"/> with the given arguments.
        /// </summary>
        /// <param name="args">The arguments.</param>
        public void Invoke(params object[] args)
        {
            if (TargetReference != null)
            {
                // Invoke method on object.
                Debug.Assert(!MethodInfo.IsStatic, "Delegate should be an object method.");
                object target = TargetReference.Target;
                if (target != null)
                    MethodInfo.Invoke(target, args);
            }
            else
            {
                // Invoke static method.
                Debug.Assert(MethodInfo.IsStatic, "Delegate should be a class method.");
                MethodInfo.Invoke(null, args);
            }
        }


        [Conditional("DEBUG")]
        private static void CheckForClosure(MethodInfo methodInfo)
        {
            if (methodInfo.DeclaringType.GetCustomAttributes(typeof(CompilerGeneratedAttribute), false).Length != 0)
                Debug.WriteLine("Warning: Creating weak-delegate to anonymous method with closure.");

        }


        [Conditional("SILVERLIGHT")]
        private static void CheckForNonPublicEventHandler(object target, MethodInfo methodInfo)
        {
            if (target != null && !target.GetType().IsPublic)

                Debug.WriteLine(
                  "Warning: Creating weak-delegate to private or internal type. The target of a " +
                  "weak-delegate or a weak-event needs to be public. This is necessary because of " +
                  "security restrictions in Silverlight.");

            if (!methodInfo.IsPublic)
                Debug.WriteLine(
                  "Warning: Creating weak-delegate to private or internal method. The target of a " +
                  "weak-delegate or a weak-event needs to be a public methods. This is necessary " +
                  "because of security restrictions in Silverlight.");
        }
        #endregion
    }
}