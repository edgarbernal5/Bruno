#include "stdafx.h"
#include "MathCollision.h"

namespace BrunoEngine
{
#ifdef BRUNO_DIRECTX
    inline bool XMVector3AnyTrue(_In_ DirectX::FXMVECTOR V)
    {
        using namespace DirectX;
        // Duplicate the fourth element from the first element.
        XMVECTOR C = XMVectorSwizzle<XM_SWIZZLE_X, XM_SWIZZLE_Y, XM_SWIZZLE_Z, XM_SWIZZLE_X>(V);

        return XMComparisonAnyTrue(XMVector4EqualIntR(C, XMVectorTrueInt()));
    }

    bool BoundingBox::Intersects(_In_ const Ray& ray, _Out_ float& fDist) const
    {
        using namespace DirectX;

        XMVECTOR Origin = XMLoadFloat3(&ray.position);
        XMVECTOR Direction = XMLoadFloat3(&ray.direction);

        // Load the box.
        XMVECTOR vCenter = XMLoadFloat3(&Center);
        XMVECTOR vExtents = XMLoadFloat3(&Extents);

        // Adjust ray origin to be relative to center of the box.
        XMVECTOR TOrigin = XMVectorSubtract(vCenter, Origin);

        // Compute the dot product againt each axis of the box.
        // Since the axii are (1,0,0), (0,1,0), (0,0,1) no computation is necessary.
        XMVECTOR AxisDotOrigin = TOrigin;
        XMVECTOR AxisDotDirection = Direction;

        // if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
        XMVECTOR IsParallel = XMVectorLessOrEqual(XMVectorAbs(AxisDotDirection), g_RayEpsilon);

        // Test against all three axii simultaneously.
        XMVECTOR InverseAxisDotDirection = XMVectorReciprocal(AxisDotDirection);
        XMVECTOR t1 = XMVectorMultiply(XMVectorSubtract(AxisDotOrigin, vExtents), InverseAxisDotDirection);
        XMVECTOR t2 = XMVectorMultiply(XMVectorAdd(AxisDotOrigin, vExtents), InverseAxisDotDirection);

        // Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
        // use the results from any directions parallel to the slab.
        XMVECTOR t_min = XMVectorSelect(XMVectorMin(t1, t2), g_FltMin, IsParallel);
        XMVECTOR t_max = XMVectorSelect(XMVectorMax(t1, t2), g_FltMax, IsParallel);

        // t_min.x = maximum( t_min.x, t_min.y, t_min.z );
        // t_max.x = minimum( t_max.x, t_max.y, t_max.z );
        t_min = XMVectorMax(t_min, XMVectorSplatY(t_min));  // x = max(x,y)
        t_min = XMVectorMax(t_min, XMVectorSplatZ(t_min));  // x = max(max(x,y),z)
        t_max = XMVectorMin(t_max, XMVectorSplatY(t_max));  // x = min(x,y)
        t_max = XMVectorMin(t_max, XMVectorSplatZ(t_max));  // x = min(min(x,y),z)

        // if ( t_min > t_max ) return false;
        XMVECTOR NoIntersection = XMVectorGreater(XMVectorSplatX(t_min), XMVectorSplatX(t_max));

        // if ( t_max < 0.0f ) return false;
        NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(XMVectorSplatX(t_max), XMVectorZero()));

        // if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return false;
        XMVECTOR ParallelOverlap = XMVectorInBounds(AxisDotOrigin, vExtents);
        NoIntersection = XMVectorOrInt(NoIntersection, XMVectorAndCInt(IsParallel, ParallelOverlap));

        if (!XMVector3AnyTrue(NoIntersection))
        {
            // Store the x-component to *pDist
            XMStoreFloat(&fDist, t_min);
            return true;
        }

        fDist = 0.f;
        return false;
    }

    _Use_decl_annotations_
        inline void BoundingBox::CreateFromPoints(BoundingBox& Out, size_t Count, const Vector3* pPoints, size_t Stride)
    {
        assert(Count > 0);
        assert(pPoints);
        using namespace DirectX;

        // Find the minimum and maximum x, y, and z
        XMVECTOR vMin, vMax;

        vMin = vMax = XMLoadFloat3(pPoints);

        for (size_t i = 1; i < Count; ++i)
        {
            XMVECTOR Point = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(reinterpret_cast<const uint8_t*>(pPoints) + i * Stride));

            vMin = XMVectorMin(vMin, Point);
            vMax = XMVectorMax(vMax, Point);
        }

        // Store center and extents.
        XMStoreFloat3(&Out.Center, XMVectorScale(XMVectorAdd(vMin, vMax), 0.5f));
        XMStoreFloat3(&Out.Extents, XMVectorScale(XMVectorSubtract(vMax, vMin), 0.5f));
    }

    _Use_decl_annotations_
    inline void BoundingSphere::CreateFromPoints(BoundingSphere& Out, size_t Count, const Vector3* pPoints, size_t Stride)
    {
        assert(Count > 0);
        assert(pPoints);

        using namespace DirectX;

        // Find the points with minimum and maximum x, y, and z
        XMVECTOR MinX, MaxX, MinY, MaxY, MinZ, MaxZ;

        MinX = MaxX = MinY = MaxY = MinZ = MaxZ = XMLoadFloat3(pPoints);

        for (size_t i = 1; i < Count; ++i)
        {
            XMVECTOR Point = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(reinterpret_cast<const uint8_t*>(pPoints) + i * Stride));

            float px = XMVectorGetX(Point);
            float py = XMVectorGetY(Point);
            float pz = XMVectorGetZ(Point);

            if (px < XMVectorGetX(MinX))
                MinX = Point;

            if (px > XMVectorGetX(MaxX))
                MaxX = Point;

            if (py < XMVectorGetY(MinY))
                MinY = Point;

            if (py > XMVectorGetY(MaxY))
                MaxY = Point;

            if (pz < XMVectorGetZ(MinZ))
                MinZ = Point;

            if (pz > XMVectorGetZ(MaxZ))
                MaxZ = Point;
        }

        // Use the min/max pair that are farthest apart to form the initial sphere.
        XMVECTOR DeltaX = XMVectorSubtract(MaxX, MinX);
        XMVECTOR DistX = XMVector3Length(DeltaX);

        XMVECTOR DeltaY = XMVectorSubtract(MaxY, MinY);
        XMVECTOR DistY = XMVector3Length(DeltaY);

        XMVECTOR DeltaZ = XMVectorSubtract(MaxZ, MinZ);
        XMVECTOR DistZ = XMVector3Length(DeltaZ);

        XMVECTOR vCenter;
        XMVECTOR vRadius;

        if (XMVector3Greater(DistX, DistY))
        {
            if (XMVector3Greater(DistX, DistZ))
            {
                // Use min/max x.
                vCenter = XMVectorLerp(MaxX, MinX, 0.5f);
                vRadius = XMVectorScale(DistX, 0.5f);
            }
            else
            {
                // Use min/max z.
                vCenter = XMVectorLerp(MaxZ, MinZ, 0.5f);
                vRadius = XMVectorScale(DistZ, 0.5f);
            }
        }
        else // Y >= X
        {
            if (XMVector3Greater(DistY, DistZ))
            {
                // Use min/max y.
                vCenter = XMVectorLerp(MaxY, MinY, 0.5f);
                vRadius = XMVectorScale(DistY, 0.5f);
            }
            else
            {
                // Use min/max z.
                vCenter = XMVectorLerp(MaxZ, MinZ, 0.5f);
                vRadius = XMVectorScale(DistZ, 0.5f);
            }
        }

        // Add any points not inside the sphere.
        for (size_t i = 0; i < Count; ++i)
        {
            XMVECTOR Point = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(reinterpret_cast<const uint8_t*>(pPoints) + i * Stride));

            XMVECTOR Delta = XMVectorSubtract(Point, vCenter);

            XMVECTOR Dist = XMVector3Length(Delta);

            if (XMVector3Greater(Dist, vRadius))
            {
                // Adjust sphere to include the new point.
                vRadius = XMVectorScale(XMVectorAdd(vRadius, Dist), 0.5f);
                vCenter = XMVectorAdd(vCenter, XMVectorMultiply(XMVectorSubtract(XMVectorReplicate(1.0f), XMVectorDivide(vRadius, Dist)), Delta));
            }
        }

        XMStoreFloat3(&Out.Center, vCenter);
        XMStoreFloat(&Out.Radius, vRadius);
    }

    _Use_decl_annotations_
     inline void BoundingBox::CreateMerged(BoundingBox& Out, const BoundingBox& b1, const BoundingBox& b2)
    {
        using namespace DirectX;

        XMVECTOR b1Center = XMLoadFloat3(&b1.Center);
        XMVECTOR b1Extents = XMLoadFloat3(&b1.Extents);

        XMVECTOR b2Center = XMLoadFloat3(&b2.Center);
        XMVECTOR b2Extents = XMLoadFloat3(&b2.Extents);

        XMVECTOR Min = XMVectorSubtract(b1Center, b1Extents);
        Min = XMVectorMin(Min, XMVectorSubtract(b2Center, b2Extents));

        XMVECTOR Max = XMVectorAdd(b1Center, b1Extents);
        Max = XMVectorMax(Max, XMVectorAdd(b2Center, b2Extents));

        assert(XMVector3LessOrEqual(Min, Max));

        XMStoreFloat3(&Out.Center, XMVectorScale(XMVectorAdd(Min, Max), 0.5f));
        XMStoreFloat3(&Out.Extents, XMVectorScale(XMVectorSubtract(Max, Min), 0.5f));
    }
#endif
}