#include "stdafx.h"
#include "Model.h"

#include "Graphics/GraphicsDevice.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"

#include "ModelMeshPart.h"
#include "ModelMesh.h"
#include "ModelBone.h"
#include "Material.h"

#include "Errors/GameException.h"

namespace BrunoEngine
{
	Model::Model(GraphicsDevice* device) : 
		m_device(device), 
		m_root(nullptr)
	{
	}

	Model::Model(GraphicsDevice* device, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, Material* material) :
		m_device(device),
		m_root(nullptr)
	{
		std::vector< ModelMeshPart*> modelMeshParts;
		ModelMeshPart* modelMeshPart = new ModelMeshPart(0, vertexBuffer->GetVertexCount(), 0, indexBuffer->GetIndexCount(), vertexBuffer, indexBuffer, material);
		modelMeshParts.push_back(modelMeshPart);
		
		Matrix transform = Matrix::Identity;
		ModelMesh* modelMesh = new ModelMesh("modelo", nullptr, transform, modelMeshParts);
		m_modelMeshes.push_back(modelMesh);
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