#include "stdafx.h"
#include "Mesh.h"

#include <assimp/scene.h>

#include "Model.h"

namespace TrioEngine
{
	Mesh::Mesh(Model& model, aiMesh& mesh)
		: m_model(&model), m_material(nullptr)
	{
		m_material = m_model->GetMaterials().at(mesh.mMaterialIndex);

		// Vertices
		m_vertices.reserve(mesh.mNumVertices);
		for (uint32_t i = 0; i < mesh.mNumVertices; i++)
		{
			m_vertices.push_back(Vector3(reinterpret_cast<const float*>(&mesh.mVertices[i])));
		}

		// Normals
		if (mesh.HasNormals())
		{
			m_normals.reserve(mesh.mNumVertices);
			for (uint32_t i = 0; i < mesh.mNumVertices; i++)
			{
				m_normals.push_back(Vector3(reinterpret_cast<const float*>(&mesh.mNormals[i])));
			}
		}

		// Tangents and Binormals
		if (mesh.HasTangentsAndBitangents())
		{
			m_tangents.reserve(mesh.mNumVertices);
			m_binormals.reserve(mesh.mNumVertices);
			for (uint32_t i = 0; i < mesh.mNumVertices; i++)
			{
				m_tangents.push_back(Vector3(reinterpret_cast<const float*>(&mesh.mTangents[i])));
				m_binormals.push_back(Vector3(reinterpret_cast<const float*>(&mesh.mBitangents[i])));
			}
		}

		// Texture Coordinates
		uint32_t uvChannelCount = mesh.GetNumUVChannels();
		for (uint32_t i = 0; i < uvChannelCount; i++)
		{
			std::vector<Vector3>* textureCoordinates = new std::vector<Vector3>();
			textureCoordinates->reserve(mesh.mNumVertices);
			m_textureCoordinates.push_back(textureCoordinates);

			aiVector3D* aiTextureCoordinates = mesh.mTextureCoords[i];
			for (uint32_t j = 0; j < mesh.mNumVertices; j++)
			{
				textureCoordinates->push_back(Vector3(reinterpret_cast<const float*>(&aiTextureCoordinates[j])));
			}
		}

		// Vertex Colors
		uint32_t colorChannelCount = mesh.GetNumColorChannels();
		for (uint32_t i = 0; i < colorChannelCount; i++)
		{
			std::vector<Vector4>* vertexColors = new std::vector<Vector4>();
			vertexColors->reserve(mesh.mNumVertices);
			m_vertexColors.push_back(vertexColors);

			aiColor4D* aiVertexColors = mesh.mColors[i];
			for (uint32_t j = 0; j < mesh.mNumVertices; j++)
			{
				vertexColors->push_back(Vector4(reinterpret_cast<const float*>(&aiVertexColors[j])));
			}
		}

		// Faces (note: could pre-reserve if we limit primitive types)
		if (mesh.HasFaces())
		{
			m_faceCount = mesh.mNumFaces;
			for (uint32_t i = 0; i < m_faceCount; i++)
			{
				aiFace* face = &mesh.mFaces[i];

				for (uint32_t j = 0; j < face->mNumIndices; j++)
				{
					m_indices.push_back(face->mIndices[j]);
				}
			}
		}
	}

	Mesh::~Mesh()
	{
		for (std::vector<Vector3>* textureCoordinates : m_textureCoordinates)
		{
			delete textureCoordinates;
		}

		for (std::vector<Vector4>* vertexColors : m_vertexColors)
		{
			delete vertexColors;
		}

		//mVertexBuffer.ReleaseBuffer();
		//mIndexBuffer.ReleaseBuffer();
	}
}