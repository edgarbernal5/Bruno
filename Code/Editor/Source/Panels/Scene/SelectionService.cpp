#include "brepch.h"
#include "SelectionService.h"

#include "Components.h"
#include "Bruno/Scene/Scene.h"
#include "Content/EditorAssetManager.h"
#include "Bruno/Renderer/Model.h"
#include "Bruno/Content/AssetManager.h"

namespace Bruno
{
	SelectionService::SelectionService(std::shared_ptr<Scene> scene, EditorAssetManager* assetManager) :
		m_scene(scene),
		m_assetManager(assetManager)
	{
	}

	void SelectionService::SelectUnderMousePosition(const Camera& camera, const Math::Int2& mousePosition)
	{
		auto ray = ConvertMousePositionToRay(camera, mousePosition);

		m_selections.clear();
		UUID entityUUID = FindEntityUUIDWithRay(ray, 1000.0f);
		if (entityUUID)
		{
			Select(entityUUID);
		}

		SelectionChanged.emit(m_selections);
	}

	void SelectionService::DeselectAll()
	{
		m_selections.clear();
	}

	Math::Matrix SelectionService::GetSelectionLocalTransform()
	{
		return m_scene->GetLocalSpaceMatrix(m_scene->GetEntityWithUUID(m_selections[0]));
	}

	Math::Matrix SelectionService::GetSelectionTransform()
	{
		return m_scene->GetWorldSpaceMatrix(m_scene->GetEntityWithUUID(m_selections[0]));
	}

	void SelectionService::Select(UUID selection)
	{
		m_selections.push_back(selection);
	}

	void SelectionService::Deselect(UUID selection)
	{
		auto it = std::find(m_selections.begin(), m_selections.end(), selection);
		if (it != m_selections.end())
		{
			m_selections.erase(it);
		}
	}

	void SelectionService::SelectEntitiesInRect(const Math::Matrix& viewProjection, const Math::Vector2& ndcRectMin,
	                                            const Math::Vector2& ndcRectMax)
	{
		// 1. Limpiamos selecciones anteriores (opcional, dependiendo de si usas Shift para sumar)
		m_scene->Clear<SelectedComponent>();

		// 2. Obtenemos todas las entidades con Transform y BoundingBox
		auto entities = m_scene->GetAllEntitiesWith<TransformComponent, BoundingBoxComponent>();

		for (auto entt : entities)
		{
			const auto& [transform, bbox] = entities.get<TransformComponent, BoundingBoxComponent>(entt);

			// Calcular matriz final: Local a Proyección
			Math::Matrix worldViewProj = transform.GetTransform() * viewProjection;

			// Obtener los 8 vértices del AABB en NDC
			Math::Vector2 entityMin(FLT_MAX, FLT_MAX);
			Math::Vector2 entityMax(-FLT_MAX, -FLT_MAX);
			bool isBehindCamera = false;

			if (ProjectAABBToNDC(bbox, worldViewProj, entityMin, entityMax, isBehindCamera))
			{
				// Si la entidad está completamente detrás de la cámara, la ignoramos
				if (isBehindCamera) continue;

				// 3. Intersección de Rectángulos 2D (AABB vs Marquee Rect)
				if (CheckRectIntersection(ndcRectMin, ndcRectMax, entityMin, entityMax))
				{
					Entity entity = { entt, m_scene.get() };
					
					// ¡Seleccionado! Le añadimos el tag a EnTT
					entity.AddComponent<SelectedComponent>();
				}
			}
		}
	}

	Math::Ray SelectionService::ConvertMousePositionToRay(Camera camera, const Math::Int2& mousePosition)
	{
		Math::Vector3 nearPoint((float)mousePosition.x, (float)mousePosition.y, 0.0f);
		Math::Vector3 farPoint((float)mousePosition.x, (float)mousePosition.y, 1.0f);

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

	UUID SelectionService::FindEntityUUIDWithRay(const Math::Ray& ray, float maxDistance)
	{
		auto entities = m_scene->GetAllEntitiesWith<IdComponent, TransformComponent, BoundingBoxComponent>();
    
		float closestDistance = (std::numeric_limits<float>::max)();
		UUID closestId = 0;

		// Nota: en EnTT es mejor iterar 'ent' por valor, no por referencia (auto&), ya que es solo un entero.
		for (auto ent : entities)
		{
			Entity entity = { ent, m_scene.get() };
        
			// Extraemos la data contigua directamente
			auto [idComponent, transformComponent, bboxComponent] = entities.get<IdComponent, TransformComponent, BoundingBoxComponent>(ent);

			Math::Matrix transform = m_scene->GetWorldSpaceMatrix(entity);

			// 2. Recreamos la caja delimitadora en espacio local
			// Asumo que tienes un constructor o inicializador para tu wrapper de BoundingBox
			Math::BoundingBox localAABB(bboxComponent.Center, bboxComponent.Extents);

			// 3. MEJORA AAA: Transformamos a un OBB (Oriented Bounding Box)
			Math::BoundingOrientedBox obb;
			Math::BoundingOrientedBox::CreateFromBoundingBox(obb, localAABB);
			obb.Transform(obb, transform);

			// 4. Test de intersección
			float distance;
			if (ray.Intersects(obb, distance) && distance <= maxDistance)
			{
				if (distance < closestDistance)
				{
					closestDistance = distance;
					closestId = idComponent.Id;
				}
			}
		}
		return closestId;
	}

	bool SelectionService::ProjectAABBToNDC(const BoundingBoxComponent& bbox, const Math::Matrix& wvp,
	                                        Math::Vector2& outMin, Math::Vector2& outMax, bool& outIsBehindCamera)
	{
		// Los 8 vértices del AABB local
        Math::Vector3 corners[8] = {
            Math::Vector3(bbox.Center.x - bbox.Extents.x, bbox.Center.y - bbox.Extents.y, bbox.Center.z - bbox.Extents.z),
            Math::Vector3(bbox.Center.x + bbox.Extents.x, bbox.Center.y - bbox.Extents.y, bbox.Center.z - bbox.Extents.z),
            Math::Vector3(bbox.Center.x - bbox.Extents.x, bbox.Center.y + bbox.Extents.y, bbox.Center.z - bbox.Extents.z),
            Math::Vector3(bbox.Center.x + bbox.Extents.x, bbox.Center.y + bbox.Extents.y, bbox.Center.z - bbox.Extents.z),
            Math::Vector3(bbox.Center.x - bbox.Extents.x, bbox.Center.y - bbox.Extents.y, bbox.Center.z + bbox.Extents.z),
            Math::Vector3(bbox.Center.x + bbox.Extents.x, bbox.Center.y - bbox.Extents.y, bbox.Center.z + bbox.Extents.z),
            Math::Vector3(bbox.Center.x - bbox.Extents.x, bbox.Center.y + bbox.Extents.y, bbox.Center.z + bbox.Extents.z),
            Math::Vector3(bbox.Center.x + bbox.Extents.x, bbox.Center.y + bbox.Extents.y, bbox.Center.z + bbox.Extents.z),
        };

        int verticesBehindCamera = 0;

        for (int i = 0; i < 8; ++i) {
            // Multiplicamos por WorldViewProjection
            // Nota: Dependiendo de tu wrapper, podrías necesitar Math::Vector4 o TransformCoord
            Math::Vector4 projected = Math::Vector4::Transform(Math::Vector4(corners[i].x, corners[i].y, corners[i].z, 1.0f), wvp);

            // Z (o W) nos dice si está detrás de la cámara (Plano Near)
            if (projected.w <= 0.0f) {
                verticesBehindCamera++;
            }

            // Normalizamos dividiendo por W (Perspectiva)
            // Cuidado con la división por cero si w es muy cercano a 0
            float invW = (projected.w > 0.0001f) ? (1.0f / projected.w) : 1.0f;
            
            float nx = projected.x * invW;
            float ny = projected.y * invW;

            // Encontramos los límites 2D del AABB proyectado en la pantalla
            if (nx < outMin.x) outMin.x = nx;
            if (nx > outMax.x) outMax.x = nx;
            if (ny < outMin.y) outMin.y = ny;
            if (ny > outMax.y) outMax.y = ny;
        }

        outIsBehindCamera = (verticesBehindCamera == 8);
        return true;
	}

	bool SelectionService::CheckRectIntersection(const Math::Vector2& rectA_Min, const Math::Vector2& rectA_Max,
		const Math::Vector2& rectB_Min, const Math::Vector2& rectB_Max)
	{
		// Prueba clásica de intersección AABB 2D
		if (rectA_Max.x < rectB_Min.x || rectA_Min.x > rectB_Max.x) return false;
		if (rectA_Max.y < rectB_Min.y || rectA_Min.y > rectB_Max.y) return false;
		return true;
	}
}
