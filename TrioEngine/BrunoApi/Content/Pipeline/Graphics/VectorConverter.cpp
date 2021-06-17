#include "stdafx.h"
#include "VectorConverter.h"

namespace TrioEngine
{
	std::map<std::string, VertexElementFormat> VectorConverter::m_typeToVertexElementFormat;
	std::map<std::string, SurfaceFormat> VectorConverter::m_typeToSurfaceFormat;
	std::map<SurfaceFormat, std::string> VectorConverter::m_surfaceFormatToType;
	std::map<VertexElementFormat, std::string> VectorConverter::m_vertexElementFormatToType;

	VertexElementFormat VectorConverter::TryGetVertexElementFormat(std::string type)
	{
		RegisterTypes();
		std::map<std::string, VertexElementFormat>::iterator itm = m_typeToVertexElementFormat.find(type);
		if (itm != m_typeToVertexElementFormat.end())
		{
			return (*itm).second;
		}
		return VertexElementFormat::Single;
	}

	void VectorConverter::RegisterTypes()
	{
		if (m_typeToVertexElementFormat.size() == 0)
		{
			RegisterType(typeid(float).name(), SurfaceFormat::Single, VertexElementFormat::Single);
			RegisterType(typeid(Vector2).name(), SurfaceFormat::Vector2, VertexElementFormat::Vector2);
			RegisterType(typeid(Vector3).name(), VertexElementFormat::Vector3);
			RegisterType(typeid(Vector4).name(), SurfaceFormat::Vector4, VertexElementFormat::Vector4);
		}
	}

	void VectorConverter::RegisterType(std::string type, SurfaceFormat surfaceFormat)
	{
		m_typeToSurfaceFormat[type] = surfaceFormat;
		m_surfaceFormatToType[surfaceFormat] = type;
	}

	void VectorConverter::RegisterType(std::string type, VertexElementFormat vertexElementFormat)
	{
		m_typeToVertexElementFormat[type] = vertexElementFormat;
		m_vertexElementFormatToType[vertexElementFormat] = type;
	}

	void VectorConverter::RegisterType(std::string type, SurfaceFormat surfaceFormat, VertexElementFormat vertexElementFormat)
	{
		RegisterType(type, surfaceFormat);
		RegisterType(type, vertexElementFormat);
	}
}