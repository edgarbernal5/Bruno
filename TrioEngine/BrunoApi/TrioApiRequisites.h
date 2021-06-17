#pragma once

#if BRUNO_WINDOWS
#ifdef BRUNO_API_EXPORTS
#define BRUNO_API_EXPORT __declspec(dllexport)
#else
#define BRUNO_API_EXPORT __declspec(dllimport)
#endif

#else

#define BRUNO_API_EXPORT

#endif

namespace BrunoEngine
{
	class Game;

	//class Game;
	//class Effect;
	//class EffectTechnique;
	//class EffectParameter;
	//class EffectPass;
	//class Scene;
	//class Transform;
	//class GameObject;
	//class Component;
	//class Object;
	//class Texture2D;
	//class RenderTarget2D;
	//class Model;
	//class ModelMesh;

	//class ContentManager;
	////class ContentReader;

	//class BuildCoordinator;
	//class TextureContent;
}