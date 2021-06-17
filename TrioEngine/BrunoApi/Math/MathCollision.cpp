#include "stdafx.h"
#include "MathCollision.h"

namespace BrunoEngine
{
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
}