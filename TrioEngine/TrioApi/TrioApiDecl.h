#pragma once

#if TRIO_WINDOWS
#ifdef TRIOAPI_EXPORTS
#define TRIOAPI_DECL __declspec(dllexport)
#else
#define TRIOAPI_DECL __declspec(dllimport)
#endif

#else

#define TRIOAPI_DECL

#endif
