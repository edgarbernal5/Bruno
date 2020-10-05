#pragma once

#include "TrioApiRequisites.h"

#include "ContentTypeWriter.h"
#include <string>

namespace TrioEngine
{
	class ModelBoneContent;
	class ModelMeshPartContent;
	class ModelContent;

	class ModelWriter : public ContentTypeWriter
	{
	public:
		ModelWriter();
		~ModelWriter();

		std::string GetWriterName() { m_name = "ModelWriter"; return m_name; }
		std::string GetReaderName() { return "ModelReader"; }
		void Write(ContentWriter *output, ContentItem* value);

	private:
		void WriteMeshes(ContentWriter* output, ModelContent* content);
		void WriteBones(ContentWriter* output, ModelContent* content);
		void WriteBoneIndex(ContentWriter* output, std::vector<ModelBoneContent*>& bones, ModelBoneContent* bone);
		void WriteMeshParts(ContentWriter* output, const std::vector<ModelMeshPartContent*>& meshParts);
	};
}