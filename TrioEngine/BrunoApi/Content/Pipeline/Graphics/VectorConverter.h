#pragma once

#include "TrioApiRequisites.h"

#include <map>
#include <string>

#include "Graphics/SurfaceFormat.h"
#include "Graphics/VertexElementFormat.h"

namespace BrunoEngine
{
	class VectorConverter
	{
	public:

		static VertexElementFormat TryGetVertexElementFormat(std::string type);

	private:
		static std::map<std::string, VertexElementFormat> m_typeToVertexElementFormat;
		static std::map<std::string, SurfaceFormat> m_typeToSurfaceFormat;
		static std::map<SurfaceFormat, std::string> m_surfaceFormatToType;
		static std::map<VertexElementFormat, std::string> m_vertexElementFormatToType;

		static void RegisterTypes();
		static void RegisterType(std::string type, SurfaceFormat surfaceFormat);
		static void RegisterType(std::string type, SurfaceFormat surfaceFormat, VertexElementFormat vertexElementFormat);
		static void RegisterType(std::string type, VertexElementFormat vertexElementFormat);
	};
}