#pragma once


#ifdef TRIO_DIRECTX

#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d11_x.h>
#define DCOMMON_H_INCLUDED
#else
#include <d3d11_1.h>
#endif

#endif

#include <Graphics/Game.h>
#include "StepTimer.h"


using namespace Cuado;

namespace Cuado {
	class IndexBuffer;
	class VertexBuffer;
	class Effect;
}

class DemoGame : public Game {
public:
	DemoGame();

protected:
	void Draw(DX::StepTimer const& timer);
	void Update(DX::StepTimer const& timer);
	void Initialize();

	IndexBuffer* m_pIndexBuffer;
	VertexBuffer* m_pVertexBuffer;
	
	Effect* m_pEffect;
};

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow);