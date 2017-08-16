#include "stdafx.h"
#include "main.h"

#include <Graphics/SimpleMath.h>

#include <Graphics/GraphicsDevice.h>
#include <Graphics/GraphicsDeviceManager.h>
#include <Graphics/Effect.h>
#include <memory>
#include <iostream>

#pragma warning(disable : 4996)

DemoGame::DemoGame() :
	Game()
{

	GraphicsDeviceManager* graphicsManager = new GraphicsDeviceManager(this);
}

void DemoGame::Initialize()
{
	Effect* effect = new Effect(GetGraphicsDevice(), "LineEffect.fx");
}

void DemoGame::Draw(DX::StepTimer const & timer)
{
	DirectX::SimpleMath::Color black(0.0, 1.0f, 1.0f);
	GetGraphicsDevice()->Clear(black);
}


void DemoGame::Update(DX::StepTimer const& timer)
{
	std::cout << "Update: dt = " << timer.GetElapsedSeconds() << "\t Total seconds: " << timer.GetTotalSeconds() << endl;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	freopen("ouput.txt", "w", stdout);

	std::unique_ptr<DemoGame> game(new DemoGame());


	game->Run();

	return 0;
}