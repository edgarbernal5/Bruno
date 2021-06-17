#include "stdafx.h"
#include "VertexContent.h"

#include "VertexChannel.h"
#include "VertexChannelCollection.h"
#include "IndirectPositionCollection.h"
#include "VertexChannelNames.h"
#include "VertexBufferContent.h"
#include "VertexDeclarationContent.h"
#include "VectorConverter.h"

namespace TrioEngine
{
	VertexContent::VertexContent(GeometryContent* geometry)
	{
		m_positionIndices = new VertexChannel("PositionIndices");
		m_positionIndices->SetTypeOf(typeid(int).name());
		m_positionIndices->SetSizeOfBase(sizeof(int));

		m_channels = new VertexChannelCollection(this);
		m_positions = new IndirectPositionCollection(geometry, m_positionIndices);
	}

	VertexContent::~VertexContent()
	{
	}

	int VertexContent::Add(int positionIndex)
	{
		m_positionIndices->Add(&positionIndex);
		return m_positionIndices->Count() - 1;
	}

	void VertexContent::AddRange(const int* positionIndexCollection, uint32_t size)
	{
		m_positionIndices->InsertRange<int>(m_positionIndices->Count(), positionIndexCollection, size);
	}

	VertexBufferContent* VertexContent::CreateVertexBuffer()
	{
		std::vector<VertexElement> list;

		//Asumimos que siempre estará presente el channel de posiciones.
		list.push_back(VertexElement(0, VertexElementFormat::Vector3, VertexElementUsage::Position, 0));
		int offset = sizeof(Vector3);

		for (int i = 0; i < m_channels->Count(); i++)
		{
			VertexElementUsage usage = VertexChannelNames::TryDecodeUsage((*m_channels)[i]->GetName());
			VertexElementFormat format = VectorConverter::TryGetVertexElementFormat((*m_channels)[i]->GetTypeOf());

			int usageIndex = VertexChannelNames::DecodeUsageIndex((*m_channels)[i]->GetName());
			list.push_back(VertexElement(offset, format, usage, usageIndex));
			offset += (*m_channels)[i]->GetSizeOfBase();
		}
		VertexBufferContent* content = new VertexBufferContent(GetVertexCount() * offset);

		for (int i = 0; i < list.size(); i++)
		{
			content->GetVertexDeclaration()->GetVertexElements().push_back(list[i]);
		}
		content->GetVertexDeclaration()->SetVertexStride(&offset);

		//Copiamos el channel de posiciones.
		content->Write<Vector3>(0, offset, &((*m_positions)[0]), m_positions->GetCount());

		int num2 = 1;
		for (int i = 0; i < m_channels->Count(); i++)
		{
			VertexElement& element = list[num2];
			VertexChannel* channel2 = (*m_channels)[i];

			std::string elementType = (*m_channels)[i]->GetTypeOf();
			int ccount = channel2->Count();

			//uint8_t* rawChannelBytes = ((*channel2)[0]);
			if (elementType == typeid(Vector2).name())
			{
				content->Write<Vector2>(element.GetOffset(), offset, reinterpret_cast<Vector2*>(((*channel2)[0])), ccount);
			}
			else if (elementType == typeid(Vector3).name())
			{
				content->Write<Vector3>(element.GetOffset(), offset, reinterpret_cast<Vector3*>(((*channel2)[0])), ccount);
			}
			else if (elementType == typeid(Vector4).name())
			{
				content->Write<Vector4>(element.GetOffset(), offset, reinterpret_cast<Vector4*>(((*channel2)[0])), ccount);
			}
			else if (elementType == typeid(float).name())
			{
				content->Write<float>(element.GetOffset(), offset, reinterpret_cast<float*>(((*channel2)[0])), ccount);
			}
			else if (elementType == typeid(double).name())
			{
				content->Write<double>(element.GetOffset(), offset, reinterpret_cast<double*>(((*channel2)[0])), ccount);
			}

			num2++;
		}
		return content;
	}

	int VertexContent::GetVertexCount()
	{
		return m_positionIndices->Count();
	}
}