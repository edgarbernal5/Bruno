#pragma once

#include "TrioApiRequisites.h"

#include <string>
#include <vector>

namespace TrioEngine
{
	class ModelBone;
	class ModelMesh;

	class GraphicsDevice;

	class TRIO_API_EXPORT Model
	{
	public:

		Model(GraphicsDevice* device);
		~Model();

		void Draw();

		friend class ModelReader;
	private:

		ModelBone* m_root;

		std::vector<Matrix> m_bonesMatrices;

		std::vector<ModelMesh*> m_modelMeshs;
		std::vector<ModelBone*> m_modelBones;

		std::map<std::string, Texture2D*> m_Textures;
		GraphicsDevice* m_device;
	};
}