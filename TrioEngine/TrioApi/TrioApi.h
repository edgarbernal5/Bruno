#pragma once

#include "TrioApiRequisites.h"

using namespace TrioEngine;

/*
BlendState
*/
extern "C" TRIO_API_EXPORT BlendState* __stdcall BlendState_Ctor();
extern "C" TRIO_API_EXPORT BlendState* __stdcall BlendState_Additive();
extern "C" TRIO_API_EXPORT BlendState* __stdcall BlendState_AlphaBlend();
extern "C" TRIO_API_EXPORT BlendState* __stdcall BlendState_NonPremultiplied();
extern "C" TRIO_API_EXPORT BlendState* __stdcall BlendState_Opaque();

/*
DepthStencilState
*/
extern "C" TRIO_API_EXPORT DepthStencilState* __stdcall DepthStencilState_Ctor();
extern "C" TRIO_API_EXPORT DepthStencilState* __stdcall DepthStencilState_Default();
extern "C" TRIO_API_EXPORT DepthStencilState* __stdcall DepthStencilState_DepthRead();
extern "C" TRIO_API_EXPORT DepthStencilState* __stdcall DepthStencilState_None();

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
extern "C" TRIO_API_EXPORT BlendState* __stdcall GraphicsDevice_GetBlendState(GraphicsDevice* device);
extern "C" TRIO_API_EXPORT DepthStencilState* __stdcall GraphicsDevice_GetDepthStencilState(GraphicsDevice* device);
extern "C" TRIO_API_EXPORT RasterizerState* __stdcall GraphicsDevice_GetRasterizerState(GraphicsDevice* device);
extern "C" TRIO_API_EXPORT Viewport __stdcall GraphicsDevice_GetViewport(GraphicsDevice* device);
extern "C" TRIO_API_EXPORT void __stdcall GraphicsDevice_Present(GraphicsDevice* device);
extern "C" TRIO_API_EXPORT void __stdcall GraphicsDevice_Reset(GraphicsDevice* device, PresentationParameters parameters);
extern "C" TRIO_API_EXPORT void __stdcall GraphicsDevice_SetBlendState(GraphicsDevice* device, BlendState* state);
extern "C" TRIO_API_EXPORT void __stdcall GraphicsDevice_SetDepthStencilState(GraphicsDevice* device, DepthStencilState* state);
extern "C" TRIO_API_EXPORT void __stdcall GraphicsDevice_SetRasterizerState(GraphicsDevice* device, RasterizerState* state);
extern "C" TRIO_API_EXPORT void __stdcall GraphicsDevice_SetViewport(GraphicsDevice* device, Viewport viewport);

/*
IndexBuffer
*/
extern "C" TRIO_API_EXPORT IndexBuffer* __stdcall IndexBuffer_Ctor(GraphicsDevice* device, int elementSize, int indexCount, int usage);
extern "C" TRIO_API_EXPORT void __stdcall IndexBuffer_SetData(IndexBuffer* buffer, uint8_t* data, uint32_t elementCount, uint32_t sizeArrayBytes);

/*
Matrix
*/
extern "C" TRIO_API_EXPORT void __stdcall Matrix_CreateLookAt(Matrix *pMatrix1, Vector3* eye, Vector3* target, Vector3* up);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_CreatePerspectiveFieldOfView(Matrix *pMatrix1, float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_CreateRotationX(Matrix *pMatrix1, float radians);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_CreateRotationY(Matrix *pMatrix1, float radians);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_CreateRotationZ(Matrix *pMatrix1, float radians);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_CreateScale(Matrix *pMatrix1, Vector3* scale);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_CreateTranslation(Matrix *pMatrix1, Vector3* translation);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_Division(Matrix *pMatrix1, Matrix *pMatrix2);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_Invert(Matrix *pMatrix);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_Multiply(Matrix *pMatrix1, Matrix *pMatrix2);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_MultiplyScalar(Matrix *pMatrix1, float scalar);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_Sub(Matrix *pMatrix1, Matrix *pMatrix2);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_Sum(Matrix *pMatrix1, Matrix *pMatrix2);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_Transpose(Matrix *pMatrix);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_CreateTRS(Matrix *pMatrix1, Vector3* position, Quaternion* rotation, Vector3* scale);
extern "C" TRIO_API_EXPORT void __stdcall Matrix_CreateWorld(Matrix *pMatrix1, Vector3* position, Vector3* forward, Vector3* up);

/*
RasterizerState
*/
extern "C" TRIO_API_EXPORT RasterizerState* __stdcall RasterizerState_Ctor();
extern "C" TRIO_API_EXPORT RasterizerState* __stdcall RasterizerState_CullClockwise();
extern "C" TRIO_API_EXPORT RasterizerState* __stdcall RasterizerState_CullCounterClockwise();
extern "C" TRIO_API_EXPORT RasterizerState* __stdcall RasterizerState_CullNone();

/*
Vector3
*/
extern "C" TRIO_API_EXPORT void __stdcall Vector3_Clamp(Vector3 *pVector1, Vector3 *pMin, Vector3 *pMax);
extern "C" TRIO_API_EXPORT void __stdcall Vector3_Cross(Vector3 *pVector1, Vector3 *pVector2);
extern "C" TRIO_API_EXPORT float __stdcall Vector3_Distance(Vector3 *pVector1, Vector3 *pVector2);
extern "C" TRIO_API_EXPORT float __stdcall Vector3_DistanceSquared(Vector3 *pVector1, Vector3 *pVector2);
extern "C" TRIO_API_EXPORT float __stdcall Vector3_Dot(Vector3 *pVector1, Vector3 *pVector2);
extern "C" TRIO_API_EXPORT void __stdcall Vector3_MultiplyDivision(Vector3 *pVector, float scalar);
extern "C" TRIO_API_EXPORT void __stdcall Vector3_MultiplyTwoVectors(Vector3 *pVector1, Vector3 *pVector2);
extern "C" TRIO_API_EXPORT void __stdcall Vector3_MultiplyScalar(Vector3 *pVector, float scalar);
extern "C" TRIO_API_EXPORT void __stdcall Vector3_Normalize(Vector3 *pVector);
extern "C" TRIO_API_EXPORT void __stdcall Vector3_SubTwoVectors(Vector3 *pVector1, Vector3 *pVector2);
extern "C" TRIO_API_EXPORT void __stdcall Vector3_SumTwoVectors(Vector3 *pVector1, Vector3 *pVector2);
extern "C" TRIO_API_EXPORT void __stdcall Vector3_UnaryNegation(Vector3 *pVector);

/*
VertexBuffer
*/
extern "C" TRIO_API_EXPORT VertexBuffer* __stdcall VertexBuffer_Ctor(GraphicsDevice* device, VertexDeclaration* vertexDeclaration, int vertexCount, int usage);
extern "C" TRIO_API_EXPORT VertexBuffer* __stdcall VertexBuffer_Ctor2(GraphicsDevice* device, VertexDeclaration* vertexDeclaration, int vertexCount);
extern "C" TRIO_API_EXPORT void __stdcall VertexBuffer_SetData(VertexBuffer* buffer, uint8_t* data, uint32_t elementCount, uint32_t sizeArrayBytes);

/*
VertexDeclaration
*/
extern "C" TRIO_API_EXPORT VertexDeclaration* __stdcall VertexDeclaration_Ctor(int vertexStride, VertexElement* elements, int sizeElements);
extern "C" TRIO_API_EXPORT VertexDeclaration* __stdcall VertexDeclaration_Ctor2(VertexElement* elements, int sizeElements);
extern "C" TRIO_API_EXPORT VertexDeclaration* __stdcall VertexDeclaration_GetP();
extern "C" TRIO_API_EXPORT VertexDeclaration* __stdcall VertexDeclaration_GetPC();
extern "C" TRIO_API_EXPORT VertexDeclaration* __stdcall VertexDeclaration_GetPNT();