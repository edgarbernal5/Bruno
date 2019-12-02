#include "stdafx.h"
#include "Object.h"

namespace TrioEngine
{
	int Object::g_id = 0;

	Object::Object()
	{
		m_id = ++g_id;
	}
}