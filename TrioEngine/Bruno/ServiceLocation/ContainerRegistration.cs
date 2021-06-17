using System;

namespace Bruno.ServiceLocation
{
    internal struct ContainerRegistration : IEquatable<ContainerRegistration>
    {
        public readonly Type Type;
        public readonly string Key;

        public ContainerRegistration(Type type, string key)
        {
            Type = type;
            Key = key;
        }

        public override bool Equals(object obj)
        {
            return obj is ContainerRegistration && this == (ContainerRegistration)obj;
        }
        public bool Equals(ContainerRegistration other)
        {
            return this == other;
        }

        public static bool operator ==(ContainerRegistration obj1, ContainerRegistration obj2)
        {
            return obj1.Type == obj2.Type && obj1.Key == obj2.Key;
        }

        public static bool operator !=(ContainerRegistration obj1, ContainerRegistration obj2)
        {
            return obj1.Type != obj2.Type || obj1.Key != obj2.Key;
        }

        public override int GetHashCode()
        {
            int hashCode = 0;
            if (Type != null)
                hashCode = Type.GetHashCode();
            if (Key != null)
                hashCode ^= Key.GetHashCode();

            return hashCode;
        }
    }
}
