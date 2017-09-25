#include "stdafx.h"
#include "main.h"

//#include <Graphics/SimpleMath.h>

#include <Graphics/GraphicsDevice.h>
#include <Graphics/GraphicsDeviceManager.h>
#include <Graphics/Effect.h>


#include <Graphics\IndexBuffer.h>
#include <Graphics\VertexBuffer.h>
#include <Graphics\VertexPositionColor.h>
#include <Graphics\VertexPosition.h>

#include <Graphics\EffectParameter.h>
#include <Graphics\EffectTechnique.h>
#include <Graphics\EffectPass.h>

#include <Graphics\RasterizerState.h>
#include <Graphics\BlendState.h>
#include <Graphics\DepthStencilState.h>

#include <memory>
#include <iostream>

#pragma warning(disable : 4996)

DemoGame::DemoGame() :
	Game(), m_pIndexBuffer(nullptr)
{
	mRadius = 5.0f;
	GraphicsDeviceManager* graphicsManager = new GraphicsDeviceManager(this);
}

void DemoGame::Initialize()
{

	Magic();
}

void DemoGame::Magic() {
	m_pEffect = new Effect(GetGraphicsDevice(), "LineEffect.fx");
	
	m_pIndexBuffer = new IndexBuffer(GetGraphicsDevice(), IndexElementSize::ThirtyTwoBits, 36, ResourceUsage::Immutable);

	uint32_t indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	m_pIndexBuffer->SetData<uint32_t>(indices, 36);
	
	Color whiteColor(1, 1, 1);
	Color blackColor(0, 0, 0);
	Color redColor(1, 0, 0);
	Color greenColor(0, 1, 0);
	Color blueColor(0, 0, 1);
	Color yellowColor(1, 0, 1);
	Color cyanColor(0, 1, 1);
	Color magentaColor(0, 0.5f, 1);

	VertexPositionColor vertexs[] =
	{
		{ Vector3(-1.0f, -1.0f, -1.0f), whiteColor },
		{ Vector3(-1.0f, +1.0f, -1.0f), blackColor },
		{ Vector3(+1.0f, +1.0f, -1.0f), redColor },
		{ Vector3(+1.0f, -1.0f, -1.0f), greenColor },
		{ Vector3(-1.0f, -1.0f, +1.0f), blueColor },
		{ Vector3(-1.0f, +1.0f, +1.0f), yellowColor },
		{ Vector3(+1.0f, +1.0f, +1.0f), cyanColor },
		{ Vector3(+1.0f, -1.0f, +1.0f), magentaColor }
	};

	Vector3 tmpConst = vertexs[0].GetPosition();
	tmpConst.x = 4.0f;
	int sizeVPC = sizeof(VertexPositionColor);
	m_pVertexBuffer = new VertexBuffer(GetGraphicsDevice(), VertexPositionColor::GetVertexDeclaration(), 8);
	m_pVertexBuffer->SetData<VertexPositionColor>(vertexs, 8);
	
}

void DemoGame::Draw(DX::StepTimer const & timer)
{
	Color LightSteelBlue(0.69f, 0.77f, 0.87f);

	auto device = GetGraphicsDevice();
	device->Clear(LightSteelBlue);

	return;
	device->SetIndexBuffer(m_pIndexBuffer);
	device->SetVertexBuffer(m_pVertexBuffer);	

	//device->SetDepthStencilState(DepthStencilState::Default);
	//device->SetBlendState(BlendState::Opaque);
	device->SetRasterizerState(RasterizerState::CullClockwise);

	Matrix view;
	Matrix proj;
	Matrix viewProj;

	float ratio = (float)device->GetPresentationParameters().GetBackBufferWidth() / device->GetPresentationParameters().GetBackBufferHeight();
	proj = Matrix::CreatePerspectiveFieldOfView(0.25f * 3.1415926535f, ratio, 0.5f, 100.0f);

	Vector3 zero(0, 0, 0);
	Vector3 up(0, 1, 0);

	view = Matrix::CreateLookAt(m_position, zero, up);

	viewProj = view*proj;

	m_pEffect->GetParameters()[0]->SetValue(viewProj);
	m_pEffect->GetTechniques()[0]->GetPasses()[0]->Apply();

	device->DrawIndexedPrimitives(PrimitiveType::TriangleList, 0, 0, 8, 0, 12);
}

void DemoGame::Update(DX::StepTimer const& timer)
{
	std::cout << "Update: dt = " << timer.GetElapsedSeconds() << "\t Total seconds: " << timer.GetTotalSeconds() << endl;

	float mTheta = 1.5f * 3.141592654f;
	float mPhi = 0.25f * 3.141592654f;
	float x = mRadius * (float)sinf(mPhi) * (float)cosf(mTheta);
	float z = mRadius * (float)sinf(mPhi) * (float)sinf(mTheta);
	float y = mRadius * (float)cosf(mPhi);

	m_position = Vector3(x, y, z);
}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	freopen("ouput.txt", "w", stdout);

	std::unique_ptr<DemoGame> game(new DemoGame());


	game->Run();

	return 0;
}