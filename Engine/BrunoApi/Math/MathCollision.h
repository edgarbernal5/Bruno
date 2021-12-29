#pragma once

#include "TrioApiRequisites.h"

#ifdef BRUNO_DIRECTX
//#include "MathVector.h"
#endif
#include <Math/MathVector.h>

namespace BrunoEngine
{
#ifdef BRUNO_DIRECTX
	struct BRUNO_API_EXPORT BoundingBox
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

		static void CreateMerged(_Out_ BoundingBox& Out, _In_ const BoundingBox& b1, _In_ const BoundingBox& b2);

		static void CreateFromPoints(_Out_ BoundingBox& Out, _In_ size_t Count,
			_In_reads_bytes_(sizeof(Vector3) + Stride * (Count - 1)) const Vector3* pPoints, _In_ size_t Stride);
	};

	struct BRUNO_API_EXPORT BoundingSphere
	{
		Vector3 Center;            // Center of the sphere.
		float Radius;               // Radius of the sphere.

		// Creators
		BoundingSphere() noexcept : Center(0, 0, 0), Radius(1.f) {}

		BoundingSphere(const BoundingSphere&) = default;
		BoundingSphere& operator=(const BoundingSphere&) = default;

		BoundingSphere(BoundingSphere&&) = default;
		BoundingSphere& operator=(BoundingSphere&&) = default;

		XM_CONSTEXPR BoundingSphere(_In_ const Vector3& center, _In_ float radius)
			: Center(center), Radius(radius) {}

		static void CreateFromPoints(_Out_ BoundingSphere& Out, _In_ size_t Count,
			_In_reads_bytes_(sizeof(Vector3) + Stride * (Count - 1)) const Vector3* pPoints, _In_ size_t Stride);
	};
#endif
}