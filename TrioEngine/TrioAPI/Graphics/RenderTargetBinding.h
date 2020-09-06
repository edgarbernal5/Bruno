#pragma once

#include "TrioApiRequisites.h"

namespace TrioEngine
{
	class Texture;

	struct TRIO_API_EXPORT RenderTargetBinding
	{
		Texture* RenderTarget;
		int ArraySlice;

		RenderTargetBinding() : RenderTarget(nullptr), ArraySlice(0) {}
		RenderTargetBinding(Texture* renderTarget) : RenderTarget(renderTarget), ArraySlice(0) {}
		RenderTargetBinding(Texture* renderTarget, int arraySlice) : RenderTarget(renderTarget), ArraySlice(arraySlice)
		{
		}

		RenderTargetBinding(const RenderTargetBinding& rhs)
		{
			RenderTarget = rhs.RenderTarget;
			ArraySlice = rhs.ArraySlice;
		}
	};

	struct TRIO_API_EXPORT RenderTargetBindings
	{
	public:
		RenderTargetBindings();
		RenderTargetBindings(RenderTargetBinding binding);
		~RenderTargetBindings();

		typedef RenderTargetBinding BaseType;

		const BaseType & operator[](int iIndex) const;

		BaseType & operator[](int iIndex);

		void clear();
		void push_back(RenderTargetBinding& parameter);
		inline size_t size();

	private:
		BaseType m_array[32];
		size_t m_n;
		size_t m_capacity;
	};

}

