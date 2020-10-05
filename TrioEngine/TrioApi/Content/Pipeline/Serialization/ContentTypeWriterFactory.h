#pragma once

#include "TrioApiRequisites.h"

#include <string>
#include <vector>

namespace TrioEngine
{
	class ContentTypeWriter;

	class ContentTypeWriterFactory
	{
	public:
		ContentTypeWriterFactory();
		~ContentTypeWriterFactory();

		template<typename T> void RegisterContentTypeWriter()
		{
			m_TypeWriters.push_back(new T());
		}

		ContentTypeWriter* CreateContentTypeInstance(std::string const& writerName)
		{
			ContentTypeWriter* writer = GetByWriterName(writerName);
			ContentTypeWriter* clone = static_cast<ContentTypeWriter *>(malloc(sizeof(writer)));
			memcpy(&clone, &writer, sizeof(writer));

			//probar con:
			//*clone = *writer; //no creo, usa el constructor copia o el operador =
			return clone;
		}
		ContentTypeWriter* GetByWriterName(std::string const& writerName);
	private:
		std::vector<ContentTypeWriter *> m_TypeWriters;
	};
}