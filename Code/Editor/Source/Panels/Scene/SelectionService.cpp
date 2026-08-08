#include "brepch.h"
#include "SelectionService.h"

#include <algorithm>

#include "Components.h"
#include "Bruno/Scene/Scene.h"
#include "Bruno/Renderer/Model.h"

namespace Bruno
{
	SelectionService::SelectionService(std::shared_ptr<Scene> scene, EditorAssetManager* assetManager) :
		m_scene(scene),
		m_assetManager(assetManager)
	{
		// Conectamos las señales de EnTT a nuestros métodos internos
		m_scene->OnConstruct<SelectedComponent>().connect<&SelectionService::OnEntitySelected>(this);
		m_scene->OnDestroy<SelectedComponent>().connect<&SelectionService::OnEntityDeselected>(this);
	}

	SelectionService::~SelectionService()
	{
		// Siempre es buena práctica desconectar al destruir el servicio
		m_scene->OnConstruct<SelectedComponent>().disconnect<&SelectionService::OnEntitySelected>(this);
		m_scene->OnDestroy<SelectedComponent>().disconnect<&SelectionService::OnEntityDeselected>(this);
	}

	void SelectionService::SelectUnderMousePosition(const Camera& camera, const Math::Int2& mousePosition)
	{
		ScopedSelectionBatch batch(*this);
		auto ray = ConvertMousePositionToRay(camera, mousePosition);

		m_scene->Clear<SelectedComponent>();
		m_selections.clear();
		
		Entity entity;
		UUID entityUUID = FindEntityUUIDWithRay(ray, 1000.0f, entity);
		if (entityUUID)
		{
			entity.AddComponent<SelectedComponent>();
		}
	}

	void SelectionService::DeselectAll()
	{
		ScopedSelectionBatch batch(*this);
		m_scene->Clear<SelectedComponent>();
		m_selections.clear();
	}

	bool SelectionService::GetGizmoTransform(Math::Matrix& outMatrix, Math::Vector3& outPosition)
	{
		auto view = m_scene->GetAllEntitiesWith<SelectedComponent, TransformComponent>();
    
		if (view.begin() == view.end())
		{
			return false;
		}

		Math::Vector3 averagePosition(0.0f, 0.0f, 0.0f);
		size_t count = 0;
		Math::Matrix lastTransform;

		for (auto entity : view)
		{
			auto& transform = view.get<TransformComponent>(entity);
			averagePosition += transform.GetWorldPosition();
			lastTransform = transform.WorldTransform;
			count++;
		}

		averagePosition /= static_cast<float>(count);
		outPosition = averagePosition;

		if (count == 1)
		{
			// Un solo objeto: El gizmo toma la matriz real del objeto (útil para Local Space rotation)
			outMatrix = lastTransform; 
		}
		else
		{
			// Múltiples objetos: Forzamos el gizmo a alinearse al Mundo (World Space)
			// pero centrado en el promedio de todas las posiciones.
			outMatrix = Math::Matrix::CreateTranslation(averagePosition);
		}

		return true;
	}

	void SelectionService::Select(UUID selection)
	{		
		auto entity = m_scene->GetEntityWithUUID(selection);
		entity.AddComponent<SelectedComponent>();
	}

	void SelectionService::Select(const std::vector<UUID>& selection)
	{
		ScopedSelectionBatch batch(*this);
		for (auto uuid : selection)
		{
			auto entity = m_scene->GetEntityWithUUID(uuid);
			entity.AddComponent<SelectedComponent>();
		}
	}

	void SelectionService::Deselect(UUID selection)
	{
		auto it = std::find(m_selections.begin(), m_selections.end(), selection);
		if (it != m_selections.end())
		{
			m_selections.erase(it);
		}
	}

	void SelectionService::SelectEntitiesInRect(const Camera& camera, const Math::Vector2& ndcRectMin, const Math::Vector2& ndcRectMax)
	{
		ScopedSelectionBatch batch(*this);
		m_scene->Clear<SelectedComponent>();
		m_selections.clear();

		float sizeX = ndcRectMax.x - ndcRectMin.x;
		float sizeY = ndcRectMax.y - ndcRectMin.y;
		if (sizeX <= 0.0001f || sizeY <= 0.0001f)
		{
			return;
		}
		
		Math::Matrix viewProj = camera.GetViewProjection();

		auto entities = m_scene->GetAllEntitiesWith<IdComponent, TransformComponent, BoundingBoxComponent>();

		for (auto ent : entities)
		{
			Entity entity = { ent, m_scene.get() };
			auto [idComponent, transformComponent, bboxComponent] = entities.get<IdComponent, TransformComponent, BoundingBoxComponent>(ent);

			Math::BoundingBox localAABB(bboxComponent.Center, bboxComponent.Extents);
			Math::BoundingOrientedBox obb;
			Math::BoundingOrientedBox::CreateFromBoundingBox(obb, localAABB);
			obb.Transform(obb, transformComponent.WorldTransform);

			Math::Vector3 worldCenter = obb.Center;
			
			// Proyectamos el centro de la entidad al Espacio Clip / NDC (-1 a 1)
			Math::Vector3 clipFloat3 = Math::Vector3::Transform(worldCenter, viewProj);

			// Validamos que el objeto esté frente a la cámara (Z en NDC para DirectX típicamente está entre 0 y 1 o -1 y 1 según el reverse-z)
			// Verificamos si las coordenadas X e Y del centro del objeto caen dentro del rectángulo del marquee en pantalla
			if (clipFloat3.x >= ndcRectMin.x && clipFloat3.x <= ndcRectMax.x &&
				clipFloat3.y >= ndcRectMin.y && clipFloat3.y <= ndcRectMax.y)
			{
				// Opcional avanzado: Si quieres asegurar que cubra toda la caja y no solo el centro, 
				// puedes proyectar los 8 vértices del OBB y verificar si al menos uno cae dentro del rectángulo.
				entity.AddComponent<SelectedComponent>();
			}
		}
	}
	
	void SelectionService::BeginBatch()
	{
		m_isBatching = true;
	}

	void SelectionService::EndBatch()
	{
		m_isBatching = false;
        
		// Solo emitimos si algo realmente cambió durante el batch
		if (m_isDirty)
		{
			SelectionChanged.emit(m_selections);
			m_isDirty = false;
		}
	}

	void SelectionService::OnEntitySelected(entt::registry& registry, entt::entity entityHandle)
	{
		Entity entity = { entityHandle, m_scene.get() };
		UUID uuid = entity.GetUUID();

		if (std::find(m_selections.begin(), m_selections.end(), uuid) == m_selections.end())
		{
			m_selections.push_back(uuid);

			if (m_isBatching)
			{
				m_isDirty = true; // Silenciamos el evento y marcamos como sucio
			}
			else
			{
				SelectionChanged.emit(m_selections); // Comportamiento normal (clic individual)
			}
		}
	}

	void SelectionService::OnEntityDeselected(entt::registry& registry, entt::entity entityHandle)
	{
		if (registry.any_of<IdComponent>(entityHandle))
		{
			UUID uuid = registry.get<IdComponent>(entityHandle).Id;
			auto it = std::find(m_selections.begin(), m_selections.end(), uuid);
            
			if (it != m_selections.end())
			{
				m_selections.erase(it);

				if (m_isBatching)
				{
					m_isDirty = true;
				}
				else
				{
					SelectionChanged.emit(m_selections);
				}
			}
		}
	}

	Math::Ray SelectionService::ConvertMousePositionToRay(Camera camera, const Math::Int2& mousePosition)
	{
		Math::Vector3 nearPoint(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), 0.0f);
		Math::Vector3 farPoint(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), 1.0f);

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

	UUID SelectionService::FindEntityUUIDWithRay(const Math::Ray& ray, float maxDistance, Entity& outEntity)
	{
		auto entities = m_scene->GetAllEntitiesWith<IdComponent, TransformComponent, BoundingBoxComponent>();
    
		float closestDistance = (std::numeric_limits<float>::max)();
		UUID closestId = 0;
		Entity closestEntity{};
		
		// Nota: en EnTT es mejor iterar 'ent' por valor, no por referencia (auto&), ya que es solo un entero.
		for (auto ent : entities)
		{
			const auto& [idComponent, transformComponent, bboxComponent] = entities.get<IdComponent, TransformComponent, BoundingBoxComponent>(ent);

			const Math::Matrix& transform = transformComponent.WorldTransform;

			Math::BoundingBox localAABB(bboxComponent.Center, bboxComponent.Extents);

			Math::BoundingOrientedBox obb;
			Math::BoundingOrientedBox::CreateFromBoundingBox(obb, localAABB);
			obb.Transform(obb, transform);

			float distance;
			if (ray.Intersects(obb, distance) && distance <= maxDistance)
			{
				if (distance < closestDistance)
				{
					closestDistance = distance;
					closestId = idComponent.Id;
					closestEntity = { ent, m_scene.get() };
				}
			}
		}
		
		outEntity = closestEntity;
		return closestId;
	}

	bool SelectionService::ProjectAABBToNDC(const BoundingBoxComponent& bbox, const Math::Matrix& wvp, Math::Vector2& outMin, Math::Vector2& outMax, bool& outIsBehindCamera)
	{
		// 1. Recrear el AABB local y obtener sus 8 esquinas físicas
		Math::BoundingBox localAABB(bbox.Center, bbox.Extents);
		Math::Vector3 corners[8];
		localAABB.GetCorners(corners);

		// Inicializamos con los extremos opuestos de la pantalla
		outMin = Math::Vector2(FLT_MAX, FLT_MAX);
		outMax = Math::Vector2(-FLT_MAX, -FLT_MAX);
    
		int verticesBehindCamera = 0;

		// 2. Proyectar CADA esquina independientemente al espacio de pantalla
		for (int i = 0; i < 8; ++i)
		{
			// Creamos un vector de 4 componentes (X, Y, Z, W=1) para la matriz de proyección
			Math::Vector4 corner4(corners[i].x, corners[i].y, corners[i].z, 1.0f);
        
			// Multiplicamos por WorldViewProjection para pasarlo a Clip Space
			Math::Vector4 p = Math::Vector4::Transform(corner4, wvp);

			// 3. Revisar si el vértice atraviesa la cámara (Z-Clipping / W-Clipping)
			if (p.w <= 0.0f)
			{
				verticesBehindCamera++;
				// IMPORTANTE: Si 'w' es negativo, la división invierte los ejes y manda
				// la geometría al otro lado de la pantalla. Le damos un valor mínimo
				// para que el Bounding Box crezca de forma segura fuera del monitor.
				p.w = 0.0001f; 
			}

			// 4. División de Perspectiva (Pasar de Clip Space a NDC puro [-1, 1])
			float invW = 1.0f / p.w;
			float ndcX = p.x * invW;
			float ndcY = p.y * invW;

			// 5. Expandir el Bounding Box 2D en base a donde cayó esta esquina
			outMin.x = std::min<int>(outMin.x, ndcX);
			outMin.y = std::min<int>(outMin.y, ndcY);
			outMax.x = std::max<int>(outMax.x, ndcX);
			outMax.y = std::max<int>(outMax.y, ndcY);
		}

		// Si TODAS las 8 esquinas están detrás de la espalda de la cámara, ignoramos el objeto
		if (verticesBehindCamera == 8)
		{
			outIsBehindCamera = true;
			return false;
		}

		outIsBehindCamera = false;
		return true;
	}

	bool SelectionService::CheckRectIntersection(const Math::Vector2& rectA_Min, const Math::Vector2& rectA_Max, const Math::Vector2& rectB_Min, const Math::Vector2& rectB_Max)
	{
		// Prueba clásica de intersección AABB 2D
		if (rectA_Max.x < rectB_Min.x || rectA_Min.x > rectB_Max.x) return false;
		if (rectA_Max.y < rectB_Min.y || rectA_Min.y > rectB_Max.y) return false;
		
		return true;
	}
}
