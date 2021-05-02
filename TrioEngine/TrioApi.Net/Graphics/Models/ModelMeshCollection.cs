
using Estero.Collections;

namespace TrioApi.Net.Graphics
{
    public class ModelMeshCollection : FixedSizeNamedList<ModelMesh>
    {
        public ModelMeshCollection(ModelMesh[] modelMeshes)
            : base(modelMeshes)
        {

        }
    }
}
