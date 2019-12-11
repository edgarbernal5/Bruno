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
Matrix
*/
void Matrix_CreateLookAt(Matrix *pMatrix1, Vector3* eye, Vector3* target, Vector3* up)
{
	pMatrix1->CreateLookAt(*eye, *target, *up);
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
	pMatrix->Invert();
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