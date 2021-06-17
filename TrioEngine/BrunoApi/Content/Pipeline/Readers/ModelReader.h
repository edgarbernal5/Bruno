#pragma once

#include "TrioApiRequisites.h"

#include "Content/ContentTypeReader.h"

#include "Math/MathVector.h"
#include <vector>

namespace BrunoEngine
{
	class Model;
	class ModelMeshPart;
	class ModelBone;

	class BRUNO_API_EXPORT ModelReader : public ContentTypeReader
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

		//Matrix GetAbsoluteTransform(Model* model);
	};
	
}