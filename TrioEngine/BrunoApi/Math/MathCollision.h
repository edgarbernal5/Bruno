#pragma once

#include "TrioApiRequisites.h"

#ifdef BRUNO_DIRECTX
//#include "MathVector.h"
#endif
#include <Math/MathVector.h>

namespace BrunoEngine
{

#ifdef BRUNO_DIRECTX
	struct BoundingBox
	{
		static const size_t CORNER_COUNT = 8;

		Vector3 Center;            // Center of the box.
		Vector3 Extents;           // Distance from the center to each side.

		 // Creators
		BoundingBox() noexcept : Center(0, 0, 0), Extents(1.f, 1.f, 1.f) {}

		BoundingBox(const BoundingBox&) = default;
		BoundingBox& operator=(const BoundingBox&) = default;

		BoundingBox(BoundingBox&&) = default;
		BoundingBox& operator=(BoundingBox&&) = default;

		constexpr BoundingBox(_In_ const Vector3& center, _In_ const Vector3& extents) noexcept
			: Center(center), Extents(extents) {}

		bool Intersects(_In_ const Ray& ray, _Out_ float& fDist) const;
	};
#endif
}