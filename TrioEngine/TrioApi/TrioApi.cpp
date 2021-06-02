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

#include "Graphics/Models/Model.h"
#include "Graphics/Models/ModelMesh.h"
#include "Graphics/Models/ModelMeshPart.h"
#include "Graphics/Models/Material.h"

#include "Scene.h"
#include "Transform.h"
#include "GameObject.h"
#include "Utils/TextureLoader.h"

#include "Content/Pipeline/Tasks/BuildCoordinator.h"
#include "Content/ContentManager.h"

#include "Renderer/Renderer.h"
#include "Renderer/RenderPaths/RenderPathForward.h"
#include <strsafe.h>

namespace TrioDLL {
	char* AllocateMemoryForString(const char* source) {
		/*
			https://limbioliong.wordpress.com/2011/06/16/returning-strings-from-a-c-api/
			https://limbioliong.wordpress.com/2011/08/14/returning-an-array-of-strings-from-c-to-c-part-1/
		*/

		//const char* szSampleString = source;

		//ULONG ulSize = strlen(szSampleString) + sizeof(char);
		//char* pszReturn = NULL;

		//pszReturn = (char*)::GlobalAlloc(GMEM_FIXED, ulSize);
		//// Copy the contents of szSampleString
		//// to the memory pointed to by pszReturn.
		//strcpy_s(pszReturn, ulSize, szSampleString);
		//// Return pszReturn.

		//return pszReturn;

		const char* szSampleString = source;
		ULONG ulSize = strlen(szSampleString) + sizeof(char);
		char* pszReturn = NULL;

		pszReturn = (char*)::CoTaskMemAlloc(ulSize);
		// Copy the contents of szSampleString
		// to the memory pointed to by pszReturn.
		strcpy(pszReturn, szSampleString);
		// Return pszReturn.
		return pszReturn;
	}
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
	BuildCoordinatorSettings* settings = new BuildCoordinatorSettings();

	if (intermediateDirectory)
	{
		settings->SetIntermediateDirectory(intermediateDirectory);
	}
	
	if (outputDirectory)
	{
		settings->SetOutputDirectory(outputDirectory);
	}

	if (rootDirectory)
	{
		settings->SetRootDirectory(rootDirectory);
	}

	return new BuildCoordinator(settings, nullptr);
}

BuildCoordinator * BuildCoordinator_Ctor2(BuildCoordinatorSettingsBridge managedSettings)
{
	BuildCoordinatorSettings* settings = new BuildCoordinatorSettings();

	if (managedSettings.IntermediateDirectory)
	{
		settings->SetIntermediateDirectory(managedSettings.IntermediateDirectory);
	}

	if (managedSettings.OutputDirectory)
	{
		settings->SetOutputDirectory(managedSettings.OutputDirectory);
	}

	if (managedSettings.RootDirectory)
	{
		settings->SetRootDirectory(managedSettings.RootDirectory);
	}

	return new BuildCoordinator(settings, nullptr);
}

void BuildCoordinator_Dtor(BuildCoordinator * buildCoordinator)
{
	delete buildCoordinator;
}
char* __stdcall BuildCoordinator_GetRelativePath(BuildCoordinator* coordinator, char* const path)
{
	return TrioDLL::AllocateMemoryForString(coordinator->GetRelativePath(path).c_str());
}

void BuildCoordinator_GetSettings(BuildCoordinator * coordinator, char* const intermediateDir, char* const outputDir, char* const rootDir)
{
	auto& settings = coordinator->GetBuildSettings();
	strcpy(intermediateDir, settings.GetIntermediateDirectory().c_str());
	/*strcpy(outputDir, settings.GetOutputDirectory().c_str());
	strcpy(rootDir, settings.GetRootDirectory().c_str());*/
}

void BuildCoordinator_GetSettings2(BuildCoordinator* coordinator, BuildCoordinatorSettingsBridge* managedSettings)
{
	auto& settings = coordinator->GetBuildSettings();

	managedSettings->IntermediateDirectory = TrioDLL::AllocateMemoryForString(settings.GetIntermediateDirectory().c_str());
	managedSettings->OutputDirectory = TrioDLL::AllocateMemoryForString(settings.GetOutputDirectory().c_str());
	managedSettings->RootDirectory = TrioDLL::AllocateMemoryForString(settings.GetRootDirectory().c_str());
}

void BuildCoordinator_RequestBuildWithoutOpaqueData(BuildCoordinator* coordinator, const char* sourceFilename, const char* assetName, const char* importerName, const char* processorName)
{
	coordinator->RequestBuild(sourceFilename, assetName, importerName, processorName, nullptr);
}

void BuildCoordinator_RequestBuild(BuildCoordinator* coordinator, const char* sourceFilename, const char* assetName, const char* importerName, const char* processorName, int opaqueDataSize, const char** opaqueDataKeys, const char** opaqueDataValues)
{
	coordinator->RequestBuild(sourceFilename, assetName, importerName, processorName, nullptr);
}

void BuildCoordinator_RunTheBuild(BuildCoordinator* coordinator)
{
	coordinator->RunTheBuild();
}

void BuildCoordinator_GetOutputFiles(BuildCoordinator * coordinator, char*** outputFilenames, int* size)
{
	auto list = coordinator->GetOutputFiles();
	auto n = list.size();

	*size = n;
	if (n > 0)
	{
		char** newArray = (char**)CoTaskMemAlloc(sizeof(char*) * n);
		for (size_t i = 0; i < n; i++)
		{
			newArray[i] = TrioDLL::AllocateMemoryForString(list[i].c_str());
		}
		*outputFilenames = newArray;
	}
}
/*
Component
*/
bool Component_GetEnabled(Component* component)
{
	return component->GetEnabled();
}

/*
ContentManager
*/
ContentManager* ContentManager_Ctor(GraphicsDevice* device)
{
	return new ContentManager(device);
}

ContentManager* ContentManager_Ctor2(GraphicsDevice* device, const char* rootDirectory)
{
	return new ContentManager(device, rootDirectory);
}

uint8_t* ContentManager_Load(ContentManager* contentManager, const char* assetName)
{
	return contentManager->Load< uint8_t >(assetName);
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
	return TrioDLL::AllocateMemoryForString(parameter->GetName());
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
	return TrioDLL::AllocateMemoryForString(pass->GetName());
}

void EffectPass_Apply(EffectPass * pass)
{
	pass->Apply();
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
	return TrioDLL::AllocateMemoryForString(technique->GetName());
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
//GameObject* GameObject_Create(const char* name)
//{
//	return GameObject::Create(name).get();
//}
//
//void GameObject_Destroy(GameObject *gameObject)
//{
//	//GameObject::Destroy(std::make_shared<GameObject>(gameObject));
//}
//
//Transform* GameObject_GetTransform(GameObject* gameObject)
//{
//	return gameObject->GetTransform().get();
//}
//
//bool GameObject_IsActiveInTree(GameObject* gameObject)
//{
//	return gameObject->IsActiveInTree();
//}
//
//bool GameObject_IsActiveSelf(GameObject* gameObject)
//{
//	return gameObject->IsActiveSelf();
//}
//
//void GameObject_SetActiveSelf(GameObject* gameObject, bool active)
//{
//	gameObject->SetActiveSelf(active);
//}

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

void GraphicsDevice_Clear(GraphicsDevice* device, float* colorAsFloat)
{
	device->Clear(colorAsFloat);
}

void GraphicsDevice_ClearAsRGBA8(GraphicsDevice * device, uint32_t packedColor)
{
	ColorRGBA8 color(packedColor);
	device->Clear(color);
}

void GraphicsDevice_DrawIndexedPrimitives(GraphicsDevice* device, PrimitiveType primitiveType, uint32_t baseVertex, uint32_t startIndex, uint32_t primitiveCount)
{
	device->DrawIndexedPrimitives(primitiveType, baseVertex, startIndex, primitiveCount);
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

void GraphicsDevice_GetPresentationParameters(GraphicsDevice* device, PresentationParameters* parameters)
{
	PresentationParameters& devideParams = device->GetPresentationParameters();
	*parameters = devideParams;
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

void GraphicsDevice_SetIndexBuffer(GraphicsDevice* device, IndexBuffer* indexBuffer)
{
	device->SetIndexBuffer(indexBuffer);
}

void GraphicsDevice_SetVertexBuffer(GraphicsDevice * device, VertexBuffer * vertexBuffer)
{
	device->SetVertexBuffer(vertexBuffer);
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

IndexBuffer* IndexBuffer_Ctor2(GraphicsDevice* device, int elementSize, int indexCount)
{
	return new IndexBuffer(device, (IndexElementSize)elementSize, indexCount);
}

void IndexBuffer_Dctor(IndexBuffer* indexBuffer)
{
	delete indexBuffer;
}

void IndexBuffer_SetData(IndexBuffer* buffer, uint8_t* data, uint32_t elementCount, uint32_t sizeArrayBytes)
{
	buffer->SetData<uint8_t>(0, data, sizeArrayBytes, 0, elementCount, SetDataOptions::None);
}

/*
Material
*/
Material* Material_Ctor(const char* name)
{
	std::string nameStr(name);
	return new Material(nameStr);
}

void Material_InsertTexture(Material* material, const char* name, Texture* texture)
{
	material->InsertTexture(name, texture);
}

/*
Matrix
*/
void Matrix_CreateFromAxisAngle(Matrix* pMatrix, Vector3* axis, float angle)
{
	*pMatrix = Matrix::CreateFromAxisAngle(*axis, angle);
}

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

void Matrix_Forward(Matrix* pMatrix, Vector3* pResult)
{
	*pResult = pMatrix->Forward();
}

void Matrix_Right(Matrix* pMatrix, Vector3* pResult)
{
	*pResult = pMatrix->Right();
}

void Matrix_Translation(Matrix* pMatrix, Vector3* pResult)
{
	*pResult = pMatrix->Translation();
}

void Matrix_Up(Matrix* pMatrix, Vector3* pResult)
{
	*pResult = pMatrix->Up();
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
Model
*/
Model* Model_Ctor(GraphicsDevice* device)
{
	return new Model(device);
}
Model* Model_Ctor2(GraphicsDevice* device, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, Material* material)
{
	return new Model(device, vertexBuffer, indexBuffer, material);
}

void Model_Dtor(Model * model)
{
	delete model;
}

void Model_GetModelMeshes(Model * model, ModelMesh ***modelMeshes, int* size)
{
	auto list = model->GetModelMeshes();
	auto n = list.size();

	*size = n;
	if (n > 0)
	{
		ModelMesh** newArray = (ModelMesh**)CoTaskMemAlloc(sizeof(ModelMesh*) * n);
		for (size_t i = 0; i < n; i++)
		{
			newArray[i] = list[i];
		}
		*modelMeshes = newArray;
	}
}

void Model_Draw(Model* model)
{
	model->Draw();
}

/*
ModelMesh
*/
void ModelMesh_Dtor(ModelMesh* modelMesh)
{
	delete modelMesh;
}

char* ModelMesh_GetName(ModelMesh* modelMesh)
{
	return TrioDLL::AllocateMemoryForString(modelMesh->GetName());
}

void ModelMesh_GetModelMeshParts(ModelMesh * modelMesh, ModelMeshPart * **modelMeshParts, int* size)
{
	auto list = modelMesh->GetModelMeshParts();
	auto n = list.size();

	*size = n;
	if (n > 0)
	{
		ModelMeshPart** newArray = (ModelMeshPart**)CoTaskMemAlloc(sizeof(ModelMeshPart*) * n);
		for (size_t i = 0; i < n; i++)
		{
			newArray[i] = list[i];
		}
		*modelMeshParts = newArray;
	}
}

/*
ModelMeshPart
*/
void ModelMeshPart_Dtor(ModelMeshPart* modelMeshPart)
{
	delete modelMeshPart;
}

VertexBuffer* ModelMeshPart_GetVertexBuffer(ModelMeshPart* modelMeshPart)
{
	return modelMeshPart->GetVertexBuffer();
}

IndexBuffer * ModelMeshPart_GetIndexBuffer(ModelMeshPart * modelMeshPart)
{
	return modelMeshPart->GetIndexBuffer();
}

Material * ModelMeshPart_GetMaterial(ModelMeshPart * modelMeshPart)
{
	return modelMeshPart->GetMaterial();
}

uint32_t ModelMeshPart_GetVertexCount(ModelMeshPart * modelMeshPart)
{
	return modelMeshPart->GetVertexCount();
}

uint32_t ModelMeshPart_GetPrimitiveCount(ModelMeshPart * modelMeshPart)
{
	return modelMeshPart->GetPrimitiveCount();
}

uint32_t ModelMeshPart_GetVertexOffset(ModelMeshPart * modelMeshPart)
{
	return modelMeshPart->GetVertexOffset();
}

uint32_t ModelMeshPart_GetStartIndex(ModelMeshPart * modelMeshPart)
{
	return modelMeshPart->GetStartIndex();
}

PrimitiveType ModelMeshPart_GetPrimitiveType(ModelMeshPart* modelMeshPart)
{
	return modelMeshPart->GetPrimitiveType();
}

/*
Object
*/
//int Object_GetId(Object* object)
//{
//	return object->GetId();
//}
//
//char* Object_GetName(Object* object)
//{
//	return TrioDLL::AllocateMemoryForString(object->GetName().c_str());
//}
//
//void Object_SetName(Object* object, const char* name)
//{
//	object->SetName(name);
//}

/*
Renderer
*/
void Renderer_DrawScene(Camera camera)
{
	//Renderer::DrawScene();
}

void Renderer_Initialize(GraphicsDevice* device)
{
	Renderer::Initialize(device);
}

void UpdatePerFrameData()
{

}

/*
RenderPath
*/
void RenderPath_Render(RenderPath* renderPath)
{
	renderPath->Render();
}

/*
RenderPathForward
*/
RenderPathForward* RenderPathForward_Ctor()
{
	return new RenderPathForward();
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
void Scene_Dtor(Scene* scene) {
	delete scene;
}

Scene* Scene_GetActiveScene()
{
	return Scene::GetActiveScene();
}

void Scene_SetActiveScene(Scene* scene)
{
	Scene::SetActiveScene(scene);
}

void Scene_Update(Scene * scene)
{
	scene->Update();
}

Camera Scene_GetCamera()
{
	return *Scene::GetCamera();
}

void Scene_UpdateCamera(Camera camera)
{
	Scene::UpdateCamera(camera);
}

void Scene_LoadFromModel(Scene* scene, Model* model)
{
	scene->LoadFromModel(model);
}

void Scene_GetHierarchies(Scene* scene, int* size, HierarchyComponentBridge** outHierarchies)
{
	ComponentManager<HierarchyComponent>& hierarchyComponents = scene->GetHierarchies();
	ComponentManager<NameComponent>& nameComponents = scene->GetNames();
	ComponentManager<TransformComponent>& tComponents = scene->GetTransforms();
	auto n = tComponents.GetCount();

	*size = n;
	if (n > 0)
	{
		HierarchyComponentBridge* newArray = (HierarchyComponentBridge*)CoTaskMemAlloc(sizeof(HierarchyComponentBridge) * n);
		for (size_t i = 0; i < n; i++)
		{
			Entity entity = tComponents.GetEntity(i);

			NameComponent& nameComp = *nameComponents.GetComponent(entity);

			HierarchyComponent* hierarchyComp = hierarchyComponents.GetComponent(entity);

			newArray[i].id = entity;
			newArray[i].parentId = hierarchyComp ? hierarchyComp->m_parentId : 0;
			newArray[i].name = TrioDLL::AllocateMemoryForString(nameComp.m_name.c_str());;
		}
		*outHierarchies = newArray;
	}
}

/*
Transform
*/
//Transform* Transform_GetParent(Transform* transform)
//{
//	return transform->GetParent().get();
//}
//
//void Transform_GetPosition(Transform* transform, Vector3 *position)
//{
//	*position = transform->GetPosition();
//}
//
//void Transform_GetRotation(Transform* transform, Quaternion *rotation)
//{
//	*rotation = transform->GetRotation();
//}
//
//void Transform_GetScale(Transform* transform, Vector3 *scale)
//{
//	*scale = transform->GetScale();
//}
//
//void Transform_SetPosition(Transform* transform, Vector3 *position)
//{
//	transform->SetPosition(*position);
//}
//
//void Transform_SetRotation(Transform* transform, Quaternion *rotation)
//{
//	transform->SetRotation(*rotation);
//}
//
//void Transform_SetScale(Transform* transform, Vector3 *scale)
//{
//	transform->SetScale(*scale);
//}
//
//void Transform_GetLocalPosition(Transform* transform, Vector3 *position)
//{
//	*position = transform->GetLocalPosition();
//}
//
//void Transform_GetLocalRotation(Transform* transform, Quaternion *rotation)
//{
//	*rotation = transform->GetRotation();
//}
//
//void Transform_GetLocalScale(Transform* transform, Vector3 *scale)
//{
//	*scale = transform->GetLocalScale();
//}
//
//void Transform_SetLocalPosition(Transform* transform, Vector3 *position)
//{
//	transform->SetLocalPosition(*position);
//}
//
//void Transform_SetLocalRotation(Transform* transform, Quaternion *rotation)
//{
//	transform->SetLocalRotation(*rotation);
//}
//
//void Transform_SetLocalScale(Transform* transform, Vector3 *scale)
//{
//	transform->SetLocalScale(*scale);
//}
//
//void Transform_SetParent(Transform* transform, Transform* parent)
//{
//	transform->SetParent(std::shared_ptr<Transform>(parent));
//}

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

Texture2D* Texture2D_CtorFromFile(GraphicsDevice* device, const char* filename)
{
	TextureLoader texLoader;
	TextureLoader::ImageInfo imgInfo = texLoader.GetTextureFromFile(filename);

	Texture2D* texture = new Texture2D(device, imgInfo.Width, imgInfo.Height);

	texture->SetData<uint8_t>(imgInfo.Data, imgInfo.SizeOfData);
	return texture;
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
Vector2
*/

float Vector2_Length(Vector2* pVector)
{
	return pVector->Length();
}

void Vector2_MultiplyScalar(Vector2* pVector, float scalar)
{
	*pVector *= scalar;
}

void Vector2_MultiplyTwoVectors(Vector2* pVector1, Vector2* pVector2)
{
	*pVector1 *= *pVector2;
}

void Vector2_SubTwoVectors(Vector2* pVector1, Vector2* pVector2)
{
	*pVector1 -= *pVector2;
}

void Vector2_SumTwoVectors(Vector2* pVector1, Vector2* pVector2)
{
	*pVector1 += *pVector2;
}

void Vector2_UnaryNegation(Vector2* pVector)
{
	*pVector = -*pVector;
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
	pVector1->Cross(*pVector2, *pVector1);
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

float Vector3_Length(Vector3 * pVector)
{
	return pVector->Length();
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

void Vector3_TransformQuat(Vector3* pVector, Quaternion* pRotation)
{
	*pVector = pVector->Transform(*pVector, *pRotation);
}

void Vector3_TransformMatrixPosition(Vector3* pPosition, Matrix* pMatrix)
{
	*pPosition = Vector3::Transform(*pPosition, *pMatrix);
}

void Vector3_TransformMatrixNormal(Vector3* pNormal, Matrix* pMatrix)
{
	*pNormal = Vector3::TransformNormal(*pNormal, *pMatrix);
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

void VertexBuffer_Dctor(VertexBuffer * vertexBuffer)
{
	delete vertexBuffer;
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

/*
Quaternion
*/
void Quaternion_CreateFromAxisAngle(Quaternion* quaternion, Vector3* pAxis, float angle)
{
	*quaternion = quaternion->CreateFromAxisAngle(*pAxis, angle);
}

void Quaternion_CreateFromMatrix(Quaternion* quaternion, Matrix* pMatrix)
{
	*quaternion = Quaternion::CreateFromRotationMatrix(*pMatrix);
}

void Quaternion_CreateFromYawPitchRoll(Quaternion * quaternion, float yaw, float pitch, float roll)
{
	*quaternion = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
}

void Quaternion_Inverse(Quaternion* quaternion)
{
	quaternion->Inverse(*quaternion);
}

void Quaternion_MultiplyTwoQuats(Quaternion * quaternion1, Quaternion * quaternion2)
{
	*quaternion1 *= *quaternion2;
}