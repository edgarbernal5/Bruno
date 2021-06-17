
using Bruno.Collections;

namespace BrunoApi.Net.Graphics
{
    public class ModelMeshCollection : FixedSizeNamedList<ModelMesh>
    {
        public ModelMeshCollection(ModelMesh[] modelMeshes)
            : base(modelMeshes)
        {

        }
    }
}
