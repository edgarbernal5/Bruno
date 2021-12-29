
using BrunoApi.Net.Graphics.Core;

namespace BrunoApi.Net.Graphics
{
	internal static class VertexDeclarationFactory<T> where T : struct, IVertexType
	{
		private static VertexDeclaration cachedDeclaration;

		public static VertexDeclaration VertexDeclaration
		{
			get
			{
				if (VertexDeclarationFactory<T>.cachedDeclaration == null)
				{
					VertexDeclarationFactory<T>.cachedDeclaration = VertexDeclaration.FromType(typeof(T));
				}
				return VertexDeclarationFactory<T>.cachedDeclaration;
			}
		}
	}
}
