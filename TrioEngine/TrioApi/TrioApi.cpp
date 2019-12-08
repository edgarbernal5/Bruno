// TrioApi.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "TrioApi.h"
#include "Game.h"
#include "Graphics/GraphicsAdapter.h"

/*
Game
*/
Game* Game_Ctor()
{
	return new Game();
}

void Game_Run(Game* game)
{
	game->Run();
}

void Game_Tick(Game* game)
{
	game->Tick();
}

/*
GraphicsAdapter
*/
void GraphicsAdapter_GetAdapters(GraphicsAdapter*** adapters, int *size)
{
	*size = 0;
	auto list = GraphicsAdapter::GetAdapters();
	auto n = list.size();

	*size = n;
	if (n > 0)
	{
		GraphicsAdapter** newArray = (GraphicsAdapter**)CoTaskMemAlloc(sizeof(GraphicsAdapter*) * n);
		for (size_t i = 0; i < n; i++)
		{
			newArray[i] = list[i];
		}
		*adapters = newArray;
	}
}

/*
GraphicsDevice
*/
GraphicsDevice* GraphicsDevice_Ctor(GraphicsAdapter* adapter, PresentationParameters parameters)
{
	return new GraphicsDevice(adapter, parameters);
}

GraphicsDevice* GraphicsDevice_Ctor2(GraphicsAdapter* adapter, int height, int width, int surfaceFormat, int depthFormat, bool isfullScreen, HWND handle)
{
	PresentationParameters parameters;
	parameters.SetBackBufferHeight(height);
	parameters.SetBackBufferWidth(width);
	parameters.SetDepthStencilFormat((DepthFormat)depthFormat);
	parameters.SetHostHWND(handle);
	parameters.SetIsFullScreen(isfullScreen);
	parameters.SetSurfaceFormat((SurfaceFormat)surfaceFormat);
	return new GraphicsDevice(adapter, parameters);
}

void GraphicsDevice_Dtor(GraphicsDevice* device)
{
	delete device;
}

void GraphicsDevice_Clear(GraphicsDevice* device, uint32_t packedColor)
{
	Color color(packedColor);
	device->Clear(color);
}

void GraphicsDevice_Present(GraphicsDevice* device)
{
	device->Present();
}

void GraphicsDevice_Reset(GraphicsDevice* device, PresentationParameters parameters)
{
	device->Reset(parameters);
}

void GraphicsDevice_SetViewport(GraphicsDevice* device, Viewport viewport)
{
	device->SetViewport(viewport);
}
