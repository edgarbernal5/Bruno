#include "stdafx.h"
#include "File.h"

#include <fstream>

namespace TrioIO
{
	bool File::Delete(const std::string& filename)
	{
		return remove(filename.c_str()) == 0;
	}

	bool File::Exist(const std::string& path)
	{
		std::ifstream file(path.c_str());
		if (file.good())
		{
			file.close();
			return true;
		}
		else
		{
			file.close();
			return false;
		}
	}

}