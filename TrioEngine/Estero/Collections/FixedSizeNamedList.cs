using System.Collections;
using System.Collections.Generic;

namespace Estero.Collections
{
    public class FixedSizeNamedList<T> : IEnumerable<T> where T : INamedItem
    {
        private T[] m_elements;
        private Dictionary<string, T> m_table;

        public int Count
        {
            get { return m_elements.Length; }
        }

        public T this[int index]
        {
            get { return m_elements[index]; }
        }

        public T this[string name]
        {
            get
            {
                return m_table[name];
            }
        }

        protected FixedSizeNamedList(T[] source)
        {
            m_elements = source;
            m_table = new Dictionary<string, T>();
            for (int i = 0; i < m_elements.Length; i++)
            {
                m_table[m_elements[i].Name] = m_elements[i];
            }
        }

        public IEnumerator<T> GetEnumerator()
        {
            return ((IEnumerable<T>)m_elements).GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return m_elements.GetEnumerator();
        }
    }
}
