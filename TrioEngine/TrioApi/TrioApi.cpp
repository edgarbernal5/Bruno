// TrioApi.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "TrioApi.h"
#include "Game.h"
#include "Graphics/GraphicsAdapter.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/VertexBuffer.h"

#include "Graphics/BlendState.h"
#include "Graphics/DepthStencilState.h"
#include "Graphics/RasterizerState.h"

#include "Graphics/VertexPosition.h"
#include "Graphics/VertexPositionColor.h"
#include "Graphics/VertexPositionNormalTexture.h"

/*
BlendState
*/
BlendState* BlendState_Ctor()
{
	return new BlendState();
}

BlendState* BlendState_Additive()
{
	return BlendState::Additive;
}

BlendState* BlendState_AlphaBlend()
{
	return BlendState::AlphaBlend;
}

BlendState* BlendState_NonPremultiplied()
{
	return BlendState::NonPremultiplied;
}

BlendState* BlendState_Opaque()
{
	return BlendState::Opaque;
}

/*
DepthStencilState
*/
DepthStencilState* DepthStencilState_Ctor()
{
	return new DepthStencilState();
}

DepthStencilState* DepthStencilState_Default()
{
	return DepthStencilState::Default;
}
DepthStencilState* DepthStencilState_DepthRead()
{
	return DepthStencilState::DepthRead;
}
DepthStencilState* DepthStencilState_None()
{
	return DepthStencilState::None;
}

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

BlendState* GraphicsDevice_GetBlendState(GraphicsDevice* device)
{
	return device->GetBlendState();
}

DepthStencilState* GraphicsDevice_GetDepthStencilState(GraphicsDevice* device)
{
	return device->GetDepthStencilState();
}

RasterizerState* GraphicsDevice_GetRasterizerState(GraphicsDevice* device)
{
	return device->GetRasterizerState();
}

Viewport GraphicsDevice_GetViewport(GraphicsDevice* device)
{
	return device->GetViewport();
}

void GraphicsDevice_Present(GraphicsDevice* device)
{
	device->Present();
}

void GraphicsDevice_Reset(GraphicsDevice* device, PresentationParameters parameters)
{
	device->Reset(parameters);
}

void GraphicsDevice_SetBlendState(GraphicsDevice* device, BlendState* state)
{
	device->SetBlendState(state);
}

void GraphicsDevice_SetDepthStencilState(GraphicsDevice* device, DepthStencilState* state)
{
	device->SetDepthStencilState(state);
}

void GraphicsDevice_SetRasterizerState(GraphicsDevice* device, RasterizerState* state)
{
	device->SetRasterizerState(state);
}

void GraphicsDevice_SetViewport(GraphicsDevice* device, Viewport viewport)
{
	device->SetViewport(viewport);
}

/*
IndexBuffer
*/
IndexBuffer* IndexBuffer_Ctor(GraphicsDevice* device, int elementSize, int indexCount, int usage)
{
	return new IndexBuffer(device, (IndexElementSize)elementSize, indexCount, (ResourceUsage)usage);
}

void IndexBuffer_SetData(IndexBuffer* buffer, uint8_t* data, uint32_t elementCount, uint32_t sizeArrayBytes)
{
	buffer->SetData<uint8_t>(0, data, sizeArrayBytes, 0, elementCount, SetDataOptions::None);
}

/*
Matrix
*/
void Matrix_CreateLookAt(Matrix *pMatrix1, Vector3* eye, Vector3* target, Vector3* up)
{
	*pMatrix1 = Matrix::CreateLookAt(*eye, *target, *up);
}

void Matrix_CreatePerspectiveFieldOfView(Matrix *pMatrix1, float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
{
	*pMatrix1 = Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance);
}

void Matrix_CreateRotationX(Matrix *pMatrix1, float radians)
{
	*pMatrix1 = Matrix::CreateRotationX(radians);
}

void Matrix_CreateRotationY(Matrix *pMatrix1, float radians)
{
	*pMatrix1 = Matrix::CreateRotationY(radians);
}

void Matrix_CreateRotationZ(Matrix *pMatrix1, float radians)
{
	*pMatrix1 = Matrix::CreateRotationZ(radians);
}

void Matrix_CreateScale(Matrix *pMatrix1, Vector3* scale)
{
	*pMatrix1 = Matrix::CreateScale(*scale);
}

void Matrix_CreateTranslation(Matrix *pMatrix1, Vector3* translation)
{
	*pMatrix1 = Matrix::CreateTranslation(*translation);
}

void Matrix_Division(Matrix *pMatrix1, Matrix *pMatrix2)
{
	*pMatrix1 /= *pMatrix2;
}

void Matrix_Invert(Matrix *pMatrix)
{
	*pMatrix = pMatrix->Invert();
}

void Matrix_Multiply(Matrix *pMatrix1, Matrix *pMatrix2)
{
	*pMatrix1 *= *pMatrix2;
}

void Matrix_MultiplyScalar(Matrix *pMatrix1, float scalar)
{
	*pMatrix1 /= scalar;
}

void Matrix_Sub(Matrix *pMatrix1, Matrix *pMatrix2)
{
	*pMatrix1 -= *pMatrix2;
}
void Matrix_Sum(Matrix *pMatrix1, Matrix *pMatrix2)
{
	*pMatrix1 += *pMatrix2;
}

void Matrix_Transpose(Matrix *pMatrix)
{
	pMatrix->Transpose();
}

void Matrix_CreateTRS(Matrix *pMatrix1, Vector3* position, Quaternion* rotation, Vector3* scale)
{
	*pMatrix1 = Matrix::CreateTRS(*position, *rotation, *scale);
}

void Matrix_CreateWorld(Matrix *pMatrix1, Vector3* position, Vector3* forward, Vector3* up)
{
	*pMatrix1 = Matrix::CreateWorld(*position, *forward, *up);
}

/*
RasterizerState
*/
RasterizerState* RasterizerState_Ctor()
{
	return new RasterizerState();
}

RasterizerState* RasterizerState_CullClockwise()
{
	return RasterizerState::CullClockwise;
}

RasterizerState* RasterizerState_CullCounterClockwise()
{
	return RasterizerState::CullCounterClockwise;
}

RasterizerState* RasterizerState_CullNone()
{
	return RasterizerState::CullNone;
}

/*
Vector3
*/
void Vector3_Clamp(Vector3 *pVector1, Vector3 *pMin, Vector3 *pMax)
{
	pVector1->Clamp(*pMin, *pMax);
}

void Vector3_Cross(Vector3 *pVector1, Vector3 *pVector2)
{
	pVector1->Cross(*pVector2);
}

float Vector3_Distance(Vector3 *pVector1, Vector3 *pVector2)
{
	return Vector3::Distance(*pVector1, *pVector2);
}

float Vector3_DistanceSquared(Vector3 *pVector1, Vector3 *pVector2)
{
	return Vector3::DistanceSquared(*pVector1, *pVector2);
}

float Vector3_Dot(Vector3 *pVector1, Vector3 *pVector2)
{
	return pVector1->Dot(*pVector2);
}

void Vector3_MultiplyDivision(Vector3 *pVector, float scalar)
{
	*pVector /= scalar;
}

void Vector3_MultiplyTwoVectors(Vector3 *pVector1, Vector3 *pVector2)
{
	*pVector1 *= *pVector2;
}

void Vector3_MultiplyScalar(Vector3 *pVector, float scalar)
{
	*pVector *= scalar;
}

void Vector3_Normalize(Vector3 *pVector)
{
	pVector->Normalize();
}

void Vector3_SubTwoVectors(Vector3 *pVector1, Vector3 *pVector2)
{
	*pVector1 -= *pVector2;
}

void Vector3_SumTwoVectors(Vector3 *pVector1, Vector3 *pVector2)
{
	*pVector1 += *pVector2;
}

void Vector3_UnaryNegation(Vector3 *pVector)
{
	*pVector = -*pVector;
}

/*
VertexBuffer
*/
VertexBuffer* VertexBuffer_Ctor(GraphicsDevice* device, VertexDeclaration* vertexDeclaration, int vertexCount, int usage)
{
	return new VertexBuffer(device, vertexDeclaration, vertexCount, (ResourceUsage)usage);
}

VertexBuffer* VertexBuffer_Ctor2(GraphicsDevice* device, VertexDeclaration* vertexDeclaration, int vertexCount)
{
	return new VertexBuffer(device, vertexDeclaration, vertexCount);
}

void VertexBuffer_SetData(VertexBuffer* buffer, uint8_t* data, uint32_t elementCount, uint32_t sizeArrayBytes)
{
	buffer->SetData<uint8_t>(data, sizeArrayBytes);
}

/*
VertexDeclaration
*/
VertexDeclaration* VertexDeclaration_Ctor(int vertexStride, VertexElement* elements, int sizeElements)
{
	auto vertices = std::vector<VertexElement>(elements, elements + sizeElements);
	return new VertexDeclaration(vertexStride, vertices);
}

VertexDeclaration* VertexDeclaration_Ctor2(VertexElement* elements, int sizeElements)
{
	auto vertices = std::vector<VertexElement>(elements, elements + sizeElements);
	return new VertexDeclaration(vertices);
}

VertexDeclaration* VertexDeclaration_GetP()
{
	return VertexPosition::GetVertexDeclaration();
}

VertexDeclaration* VertexDeclaration_GetPC()
{
	return VertexPositionColor::GetVertexDeclaration();
}

VertexDeclaration* VertexDeclaration_GetPNT()
{
	return VertexPositionNormalTexture::GetVertexDeclaration();
}