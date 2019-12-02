#include "stdafx.h"
#include "Component.h"

#include "GameObject.h"

namespace TrioEngine
{
	Component::Component()
	{

	}

	Component::~Component()
	{

	}

	const std::shared_ptr<Transform>& Component::GetTransform() const
	{
		return GetGameObject()->GetTransform();
	}
}