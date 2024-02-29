#include "SelectionService.h"

#include "Bruno/Scene/Scene.h"
#include "Content/EditorAssetManager.h"
#include "Bruno/Renderer/Model.h"
#include "Bruno/Content/AssetManager.h"

namespace Bruno
{
	SelectionService::SelectionService(std::shared_ptr<Scene> scene, AbstractAssetManager* assetManager) :
		m_scene(scene),
		m_assetManager(assetManager)
	{
	}

	void SelectionService::Select(UUID selection)
	{
		m_selections.clear();
		m_selections.push_back(selection);
	}

	void SelectionService::SelectUnderMousePosition(const Camera& camera, const Math::Int2& mousePosition)
	{
		m_selections.clear();

		auto ray = ConvertMousePositionToRay(camera, mousePosition);

		UUID entityUUID = FindEntityUUIDWithRay(ray, 1000.0f);
		if (entityUUID) {
			Select(entityUUID);
		}
	}

	void SelectionService::DeselectAll()
	{
		m_selections.clear();
	}

	UUID SelectionService::FindEntityUUIDWithRay(const Math::Ray& ray, float maxDistance)
	{
		auto entities = m_scene->GetAllEntitiesWith<IdComponent, ModelComponent>();
		float closestDistance = (std::numeric_limits<float>::max)();
		UUID closestId = 0;
		for (auto& ent : entities)
		{
			Entity entity = { ent, m_scene.get() };
			auto [idComponent, modelComponent] = entities.get<IdComponent, ModelComponent>(ent);
			auto model = m_assetManager->GetAsset<Model>(modelComponent.ModelHandle);

			uint32_t meshIndex = modelComponent.MeshIndex;
			auto& meshes = model->GetMeshes();
			auto& mesh = meshes[meshIndex];
			auto bbox = mesh->GetBoundingBox();

			Math::Matrix transform = m_scene->GetWorldSpaceMatrix(entity);

			Math::Vector3 bboxCenter = bbox.Center;
			Math::Vector3 bboxExtents = bbox.Extents;

			auto bboxMin = bboxCenter - bboxExtents;
			auto bboxMax = bboxCenter + bboxExtents;
			bboxMin = Math::Vector3::Transform(bboxMin, transform);
			bboxMax = Math::Vector3::Transform(bboxMax, transform);

			bboxCenter.x = std::min<float>(bboxMin.x, bboxMax.x);
			bboxCenter.y = std::min<float>(bboxMin.y, bboxMax.y);
			bboxCenter.z = std::min<float>(bboxMin.z, bboxMax.z);
			bboxExtents.x = std::max<float>(bboxMin.x, bboxMax.x);
			bboxExtents.y = std::max<float>(bboxMin.y, bboxMax.y);
			bboxExtents.z = std::max<float>(bboxMin.z, bboxMax.z);

			bbox.Center = (bboxExtents + bboxCenter) * 0.5f;
			bbox.Extents = (bboxExtents - bboxCenter) * 0.5f;
			
			float distance;
			if (ray.Intersects(bbox, distance) && distance <= maxDistance)
			{
				if (closestDistance > distance)
				{
					closestDistance = distance;
					closestId = idComponent.Id;
				}
			}
		}
		return closestId;
	}

	Math::Matrix SelectionService::GetSelectionLocalTransform()
	{
		return m_scene->GetLocalSpaceMatrix(m_scene->GetEntityWithUUID(m_selections[0]));
	}

	Math::Matrix SelectionService::GetSelectionTransform()
	{
		return m_scene->GetWorldSpaceMatrix(m_scene->GetEntityWithUUID(m_selections[0]));
	}

	Math::Ray SelectionService::ConvertMousePositionToRay(Camera camera, const Math::Int2& mousePosition)
	{
		Math::Vector3 nearPoint(mousePosition.x, mousePosition.y, 0.0f);
		Math::Vector3 farPoint(mousePosition.x, mousePosition.y, 1.0f);

		nearPoint = camera.GetViewport().Unproject(nearPoint,
			camera.GetProjection(),
			camera.GetView(),
			Math::Matrix::Identity);

		farPoint = camera.GetViewport().Unproject(farPoint,
			camera.GetProjection(),
			camera.GetView(),
			Math::Matrix::Identity);

		Math::Vector3 direction = farPoint - nearPoint;
		direction.Normalize();

		return Math::Ray(nearPoint, direction);
	}
}