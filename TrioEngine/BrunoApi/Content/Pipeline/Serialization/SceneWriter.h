#pragma once

#include "TrioApiRequisites.h"

#include "ContentTypeWriter.h"
#include <string>

namespace BrunoEngine
{
	class ModelBoneContent;
	class SceneSubMeshContent;
	class SceneContent;

	class SceneWriter : public ContentTypeWriter
	{
	public:
		SceneWriter();
		~SceneWriter();

		std::string GetWriterName() { m_name = "SceneWriter"; return m_name; }
		std::string GetReaderName() { return "SceneReader"; }
		void Write(ContentWriter *output, ContentItem* value);

	private:
		void WriteBones(ContentWriter* output, SceneContent* content);
		void WriteMaterials(ContentWriter* output, SceneContent* content);
		void WriteMeshes(ContentWriter* output, SceneContent* content);
		void WriteBoneIndex(ContentWriter* output, std::vector<ModelBoneContent*>& bones, ModelBoneContent* bone);
		void WriteSubMeshes(ContentWriter* output, const std::vector<SceneSubMeshContent*>& meshParts);
	};
}