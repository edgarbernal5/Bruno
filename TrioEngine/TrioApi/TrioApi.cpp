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

#include "Graphics/Effect.h"
#include "Graphics/EffectTechnique.h"
#include "Graphics/EffectPass.h"
#include "Graphics/EffectParameter.h"

#include "Graphics/VertexPosition.h"
#include "Graphics/VertexPositionColor.h"
#include "Graphics/VertexPositionNormalTexture.h"
#include "Graphics/Texture2D.h"
#include "Graphics/RenderTarget2D.h"

#include "Scene.h"
#include "Transform.h"
#include "GameObject.h"
#include "Utils/TextureLoader.h"

#include "Content/Pipeline/Tasks/BuildCoordinator.h"

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
BuildCoordinator
*/
BuildCoordinator* BuildCoordinator_Ctor(const char* intermediateDirectory, const char* outputDirectory, const char* rootDirectory)
{
	BuildCoordinatorSettings settings;

	if (intermediateDirectory)
	{
		settings.IntermediateDirectory = intermediateDirectory;
	}
	
	if (outputDirectory)
	{
		settings.OutputDirectory = outputDirectory;
	}

	if (rootDirectory)
	{
		settings.RootDirectory = rootDirectory;
	}

	return new BuildCoordinator(settings, nullptr);
}

void BuildCoordinator_RequestBuild(BuildCoordinator* coordinator, const char* sourceFilename, const char* assetName, const char* importerName, const char* processorName, int opaqueDataSize, const char** opaqueDataKeys, const char** opaqueDataValues)
{
	coordinator->RequestBuild(sourceFilename, assetName, importerName, processorName, nullptr);
}

void BuildCoordinator_RunTheBuild(BuildCoordinator* coordinator)
{
	coordinator->RunTheBuild();
}

/*
Component
*/
bool Component_GetEnabled(Component* component)
{
	return component->GetEnabled();
}

/*
Effect
*/
Effect* Effect_Ctor(GraphicsDevice* device)
{
	return new Effect(device);
}

void Effect_CompileEffectFromFile(Effect* effect, const char* filename)
{
	effect->CompileEffectFromFile(filename);
}

void Effect_GetParameters(Effect* effect, EffectParameter*** parameters, int *size)
{
	auto list = effect->GetParameters();
	auto n = list.size();

	*size = n;
	if (n > 0)
	{
		EffectParameter** newArray = (EffectParameter**)CoTaskMemAlloc(sizeof(EffectParameter*) * n);
		for (size_t i = 0; i < n; i++)
		{
			newArray[i] = list[i];
		}
		*parameters = newArray;
	}
}

void Effect_GetTechniques(Effect* effect, EffectTechnique*** techniques, int *size)
{
	auto list = effect->GetTechniques();
	auto n = list.size();

	*size = n;
	if (n > 0)
	{
		EffectTechnique** newArray = (EffectTechnique**)CoTaskMemAlloc(sizeof(EffectTechnique*) * n);
		for (size_t i = 0; i < n; i++)
		{
			newArray[i] = list[i];
		}
		*techniques = newArray;
	}
}

/*
EffectParameter
*/
char* EffectParameter_GetName(EffectParameter* parameter)
{
	const char *szSampleString = parameter->GetName();

	ULONG ulSize = strlen(szSampleString) + sizeof(char);
	char* pszReturn = NULL;

	pszReturn = (char*)::GlobalAlloc(GMEM_FIXED, ulSize);
	// Copy the contents of szSampleString
	// to the memory pointed to by pszReturn.
	strcpy_s(pszReturn, ulSize, szSampleString);
	// Return pszReturn.

	return pszReturn;
}
void EffectParameter_SetValueScalar(EffectParameter* parameter, float scalar)
{
	parameter->SetValue(scalar);
}

void EffectParameter_SetValueMatrix(EffectParameter* parameter, Matrix* matrix)
{
	parameter->SetValue(*matrix);
}

void EffectParameter_SetValueVector2(EffectParameter* parameter, Vector2* vector)
{
	parameter->SetValue(*vector);
}

void EffectParameter_SetValueVector3(EffectParameter* parameter, Vector3* vector)
{
	parameter->SetValue(*vector);
}

void EffectParameter_SetValueVector4(EffectParameter* parameter, Vector4* vector)
{
	parameter->SetValue(*vector);
}

void EffectParameter_SetValueColor(EffectParameter* parameter, uint32_t packedColor)
{
	Color color(packedColor);
	parameter->SetValue(color);
}

/*
EffectPass
*/
char* EffectPass_GetName(EffectPass* pass)
{
	const char *szSampleString = pass->GetName();

	ULONG ulSize = strlen(szSampleString) + sizeof(char);
	char* pszReturn = NULL;

	pszReturn = (char*)::GlobalAlloc(GMEM_FIXED, ulSize);
	// Copy the contents of szSampleString
	// to the memory pointed to by pszReturn.
	strcpy_s(pszReturn, ulSize, szSampleString);
	// Return pszReturn.

	return pszReturn;
}

/*
EffectTechnique
*/
void EffectTechnique_GetPasses(EffectTechnique* technique, EffectPass*** passes, int *size)
{
	auto list = technique->GetPasses();
	auto n = list.size();

	*size = n;
	if (n > 0)
	{
		EffectPass** newArray = (EffectPass**)CoTaskMemAlloc(sizeof(EffectPass*) * n);
		for (size_t i = 0; i < n; i++)
		{
			newArray[i] = list[i];
		}
		*passes = newArray;
	}
}

char* EffectTechnique_GetName(EffectTechnique* technique)
{
	const char *szSampleString = technique->GetName();

	ULONG ulSize = strlen(szSampleString) + sizeof(char);
	char* pszReturn = NULL;

	pszReturn = (char*)::GlobalAlloc(GMEM_FIXED, ulSize);
	// Copy the contents of szSampleString
	// to the memory pointed to by pszReturn.
	strcpy_s(pszReturn, ulSize, szSampleString);
	// Return pszReturn.

	return pszReturn;
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
GameObject
*/
GameObject* GameObject_Create(const char* name)
{
	return GameObject::Create(name).get();
}

void GameObject_Destroy(GameObject *gameObject)
{
	//GameObject::Destroy(std::make_shared<GameObject>(gameObject));
}

Transform* GameObject_GetTransform(GameObject* gameObject)
{
	return gameObject->GetTransform().get();
}

bool GameObject_IsActiveInTree(GameObject* gameObject)
{
	return gameObject->IsActiveInTree();
}

bool GameObject_IsActiveSelf(GameObject* gameObject)
{
	return gameObject->IsActiveSelf();
}

void GameObject_SetActiveSelf(GameObject* gameObject, bool active)
{
	gameObject->SetActiveSelf(active);
}

/*
GraphicsAdapter
*/
void GraphicsAdapter_GetAdapters(GraphicsAdapter*** adapters, int *size)
{
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

void GraphicsDevice_SetRenderTarget(GraphicsDevice* device, RenderTarget2D* renderTarget)
{
	device->SetRenderTarget(renderTarget);
}

void GraphicsDevice_SetRenderTargets(GraphicsDevice * device, RenderTarget2D * *renderTarget, int size)
{

}

void GraphicsDevice_Flush(GraphicsDevice * device)
{
	device->Flush();
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
Object
*/
int Object_GetId(Object* object)
{
	return object->GetId();
}

char* Object_GetName(Object* object)
{
	const char *szSampleString = object->GetName().c_str();

	ULONG ulSize = strlen(szSampleString) + sizeof(char);
	char* pszReturn = NULL;

	pszReturn = (char*)::GlobalAlloc(GMEM_FIXED, ulSize);
	// Copy the contents of szSampleString
	// to the memory pointed to by pszReturn.
	strcpy_s(pszReturn, ulSize, szSampleString);
	// Return pszReturn.

	return pszReturn;
}

void Object_SetName(Object* object, const char* name)
{
	object->SetName(name);
}

/*
RenderTarget2D
*/
RenderTarget2D* RenderTarget2D_Ctor(GraphicsDevice* device, int width, int height, SurfaceFormat preferredFormat)
{
	return new RenderTarget2D(device, width, height, preferredFormat);
}

RenderTarget2D* RenderTarget2D_Ctor2(GraphicsDevice* device, int width, int height, uint32_t mipmapLevels, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, RenderTargetUsage usage)
{
	return new RenderTarget2D(device, width, height, mipmapLevels, preferredFormat, preferredDepthFormat, usage);
}

RenderTarget2D* RenderTarget2D_Ctor3(GraphicsDevice* device, int width, int height, uint32_t mipmapLevels, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat)
{
	return new RenderTarget2D(device, width, height, mipmapLevels, preferredFormat, preferredDepthFormat);
}

void RenderTarget2D_Dctor(RenderTarget2D* renderTarget)
{
	delete renderTarget;
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
Scene
*/
Scene* Scene_Ctor()
{
	return new Scene();
}

Scene* Scene_GetActiveScene()
{
	return Scene::ActiveScene();
}

/*
Transform
*/
Transform* Transform_GetParent(Transform* transform)
{
	return transform->GetParent().get();
}

void Transform_GetPosition(Transform* transform, Vector3 *position)
{
	*position = transform->GetPosition();
}

void Transform_GetRotation(Transform* transform, Quaternion *rotation)
{
	*rotation = transform->GetRotation();
}

void Transform_GetScale(Transform* transform, Vector3 *scale)
{
	*scale = transform->GetScale();
}

void Transform_SetPosition(Transform* transform, Vector3 *position)
{
	transform->SetPosition(*position);
}

void Transform_SetRotation(Transform* transform, Quaternion *rotation)
{
	transform->SetRotation(*rotation);
}

void Transform_SetScale(Transform* transform, Vector3 *scale)
{
	transform->SetScale(*scale);
}

void Transform_GetLocalPosition(Transform* transform, Vector3 *position)
{
	*position = transform->GetLocalPosition();
}

void Transform_GetLocalRotation(Transform* transform, Quaternion *rotation)
{
	*rotation = transform->GetRotation();
}

void Transform_GetLocalScale(Transform* transform, Vector3 *scale)
{
	*scale = transform->GetLocalScale();
}

void Transform_SetLocalPosition(Transform* transform, Vector3 *position)
{
	transform->SetLocalPosition(*position);
}

void Transform_SetLocalRotation(Transform* transform, Quaternion *rotation)
{
	transform->SetLocalRotation(*rotation);
}

void Transform_SetLocalScale(Transform* transform, Vector3 *scale)
{
	transform->SetLocalScale(*scale);
}

void Transform_SetParent(Transform* transform, Transform* parent)
{
	transform->SetParent(std::shared_ptr<Transform>(parent));
}

/*
Texture2D
*/
SurfaceFormat Texture_GetFormat(Texture* texture)
{
	return texture->GetFormat();
}

/*
Texture2D
*/
Texture2D* Texture2D_Ctor(GraphicsDevice* device, int width, int height)
{
	return new Texture2D(device, width, height);
}

Texture2D* Texture2D_Ctor2(GraphicsDevice* device, int width, int height, SurfaceFormat format)
{
	return new Texture2D(device, width, height, format);
}

void Texture2D_Dctor(Texture2D* texture)
{
	delete texture;
}

void Texture2D_GetData(Texture2D* texture, uint8_t* data, uint32_t elementCount)
{
	texture->GetData(0, nullptr, data, 0, elementCount);
}

void Texture2D_TestLoadFromFile(Texture2D * texture)
{
	TextureLoader texLoader;
	TextureLoader::ImageInfo imgInfo = texLoader.GetTextureFromFile("D:\\Edgar\\Documentos\\Proyectos\\CG\\TrioEngineGit\\TrioEngine\\TrioWin32Sample\\edgar.jpg");

	texture->SetData<uint8_t>(imgInfo.Data, imgInfo.SizeOfData);
}

int Texture2D_GetWidth(Texture2D* texture)
{
	return texture->GetWidth();
}

int Texture2D_GetHeight(Texture2D* texture)
{
	return texture->GetHeight();
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