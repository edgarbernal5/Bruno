#include "stdafx.h"
#include "Material.h"

namespace TrioEngine
{
	Material::Material()
	{
	}

	Material::Material(std::string& name) :
		m_name(name)
	{

	}

	Material::~Material()
	{
	}
}