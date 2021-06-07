
using TrioApi.Net.Maths;

namespace TrioApi.Net.Game
{
    public struct SceneTransform
    {
        public Matrix WorldMatrix;

        public Vector3 LocalPosition;
        public Vector3 LocalScale;
        public Quaternion LocalRotation;
    }
}
