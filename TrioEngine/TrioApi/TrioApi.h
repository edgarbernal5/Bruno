#pragma once

#include "TrioApiRequisites.h"

using namespace TrioEngine;

/*
Game
*/
extern "C" TRIO_API_EXPORT Game* __stdcall Game_Ctor();
extern "C" TRIO_API_EXPORT void __stdcall Game_Run(Game* game);
extern "C" TRIO_API_EXPORT void __stdcall Game_Tick(Game* game);

/*
GraphicsAdapter
*/
extern "C" TRIO_API_EXPORT void __stdcall GraphicsAdapter_GetAdapters(GraphicsAdapter*** adapters, int *size);

/*
GraphicsDevice
*/
extern "C" TRIO_API_EXPORT GraphicsDevice* __stdcall GraphicsDevice_Ctor(GraphicsAdapter* adapter, PresentationParameters parameters);
extern "C" TRIO_API_EXPORT GraphicsDevice* __stdcall GraphicsDevice_Ctor2(GraphicsAdapter* adapter, int height, int width, int surfaceFormat, int depthFormat, bool isfullScreen, HWND handle);
extern "C" TRIO_API_EXPORT void __stdcall GraphicsDevice_Dtor(GraphicsDevice* device);
extern "C" TRIO_API_EXPORT void __stdcall GraphicsDevice_Clear(GraphicsDevice* device, uint32_t color);
extern "C" TRIO_API_EXPORT void __stdcall GraphicsDevice_Present(GraphicsDevice* device);
extern "C" TRIO_API_EXPORT void __stdcall GraphicsDevice_Reset(GraphicsDevice* device, PresentationParameters parameters);
extern "C" TRIO_API_EXPORT void __stdcall GraphicsDevice_SetViewport(GraphicsDevice* device, Viewport viewport);