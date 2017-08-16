#pragma once

#ifdef TRIOAPI_EXPORTS
#define TRIOAPI_DLL __declspec(dllexport) 
#else
#define TRIOAPI_DLL __declspec(dllimport) 
#endif


