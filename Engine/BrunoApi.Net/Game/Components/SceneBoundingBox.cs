using BrunoApi.Net.Maths;

namespace BrunoApi.Net.Game.Components
{
    public struct SceneBoundingBox
    {
        public Vector3 Center;
        public Vector3 Extents;

        public SceneBoundingBox(Vector3 center, Vector3 extents)
        {
            Center = center;
            Extents = extents;
        }
    }
}
