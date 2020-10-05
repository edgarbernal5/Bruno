#pragma once

#include "TrioApiRequisites.h"

#include "Content/ContentTypeReader.h"

#include <vector>

namespace TrioEngine
{
	class Model;
	class ModelMeshPart;
	class ModelBone;

	class TRIO_API_EXPORT ModelReader : public ContentTypeReader
	{
	public:
		ModelReader();
		~ModelReader();

		const char* GetReaderName() { return "ModelReader"; }
		void* Read(ContentReader* input);

	private:
		void ReadBones(Model* model, ContentReader* input);
		void ReadMeshes(Model* model, ContentReader* input);
		std::vector<ModelMeshPart*> ReadMeshParts(ContentReader* input);
		ModelBone* ReadBoneReference(Model* model, ContentReader* input);
	};
	
}