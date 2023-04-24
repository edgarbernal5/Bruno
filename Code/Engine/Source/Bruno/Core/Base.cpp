#include "brpch.h"
#include "Base.h"

#define BRUNO_BUILD_ID "v0.1"

namespace Bruno
{
	void InitializeCore()
	{
		Log::Initialize();

		BR_CORE_INFO << "Bruno Engine " << BRUNO_BUILD_ID << std::endl;
		BR_CORE_INFO << "Initializing..." << std::endl;
	}

	void ShutdownCore()
	{
		BR_CORE_INFO << "Shutting down... " << std::endl;

		Log::Shutdown();
	}
}