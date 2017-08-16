

#ifdef TRIOFX_EXPORTS
#define TRIOFXAPI_DLL __declspec(dllexport) 
#else
#define TRIOFXAPI_DLL __declspec(dllimport) 
#endif