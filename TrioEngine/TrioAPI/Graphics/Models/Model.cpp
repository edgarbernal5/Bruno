#include "stdafx.h"
#include "Model.h"

#include "Graphics/GraphicsDevice.h"
#include "ModelMeshPart.h"
#include "ModelMesh.h"
#include "ModelBone.h"

#include "Errors/GameException.h"

namespace TrioEngine
{
	Model::Model(GraphicsDevice* device) : 
		m_device(device), 
		m_root(nullptr)
	{
	}

	Model::~Model()
	{

	}

	void Model::Draw()
	{
		auto meshCount = m_modelMeshes.size();

		for (size_t i = 0; i < meshCount; i++)
		{
			ModelMesh* mesh = m_modelMeshes[i];
			for (size_t j = 0; j < mesh->GetModelMeshParts().size(); j++)
			{
				ModelMeshPart* meshPart = mesh->GetModelMeshParts()[j];
				if (meshPart->GetVertexCount() > 0)
				{
					m_device->SetVertexBuffer(meshPart->GetVertexBuffer());
					m_device->SetIndexBuffer(meshPart->GetIndexBuffer());

					m_device->DrawIndexedPrimitives(meshPart->GetPrimitiveType(), meshPart->GetVertexOffset(), meshPart->GetStartIndex(), meshPart->GetPrimitiveCount());

					//Obtener datos estadísticos.
				}
			}
		}
	}
}