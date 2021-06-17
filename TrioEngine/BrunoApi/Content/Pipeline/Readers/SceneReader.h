#pragma once

#include "TrioApiRequisites.h"

#include "Content/ContentTypeReader.h"

#include <vector>

namespace BrunoEngine
{
	class Scene;
	class Model;
	class ModelMeshPart;
	class ModelBone;

	class BRUNO_API_EXPORT SceneReader : public ContentTypeReader
	{
	public:
		SceneReader();
		~SceneReader();

		const char* GetReaderName() { return "SceneReader"; }
		void* Read(ContentReader* input);

	private:
		void ReadBones(Scene* scene, ContentReader* input);
		void ReadMaterials(Scene* scene, ContentReader* input);
		void ReadMeshes(Scene* scene, ContentReader* input);
		std::vector<ModelMeshPart*> ReadMeshParts(ContentReader* input);
		ModelBone* ReadBoneReference(Scene* scene, ContentReader* input);
	};
	
}