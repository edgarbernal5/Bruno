#pragma once

#if TRIO_WINDOWS
#ifdef TRIOAPI_EXPORTS
#define TRIO_API_EXPORT __declspec(dllexport)
#else
#define TRIO_API_EXPORT __declspec(dllimport)
#endif

#else

#define TRIO_API_EXPORT

#endif
namespace TrioEngine
{
	class Game;
	class Effect;
	class EffectTechnique;
	class EffectParameter;
	class EffectPass;
}