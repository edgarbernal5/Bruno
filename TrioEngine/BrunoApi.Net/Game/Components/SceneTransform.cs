
using BrunoApi.Net.Maths;

namespace BrunoApi.Net.Game.Components
{
    public struct SceneTransform
    {
        public Matrix WorldMatrix;

        public Vector3 LocalPosition;
        public Vector3 LocalScale;
        public Quaternion LocalRotation;
    }
}
