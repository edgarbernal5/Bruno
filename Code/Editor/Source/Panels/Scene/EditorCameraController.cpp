#include "brepch.h"
#include "EditorCameraController.h"

#include "Bruno/Scene/Scene.h"

namespace Bruno
{
    EditorCameraController::EditorCameraController(Camera& camera, Scene* scene, SelectionService* selectionService) :
		m_camera(camera),
		m_scene(scene),
		m_selectionService(selectionService)
    {
    }

    void EditorCameraController::FocusOnSelection()
    {
		// 1. Obtener entidades seleccionadas de tu SelectionService
		const auto& selections = m_selectionService->GetSelections();
		if (selections.empty())
		{
			return;
		}
		
		DirectX::BoundingBox mergedBox;
		bool hasBounds = false;

		// 2. Iterar sobre la selección para calcular la caja delimitadora global
		for (auto uuid : selections)
		{
			Entity entity = m_scene->GetEntityWithUUID(uuid);
			if (entity.HasComponent<BoundingBoxComponent>())
			{
				const auto& bboxComp = entity.GetComponent<BoundingBoxComponent>();
            
				// Reconstruimos el AABB o usamos el OBB transformado
				DirectX::BoundingBox entityBox(bboxComp.Center, bboxComp.Extents);

				if (!hasBounds)
				{
					mergedBox = entityBox;
					hasBounds = true;
				}
				else
				{
					// Magia de DirectXMath: Fusiona la caja actual con la nueva
					DirectX::BoundingBox::CreateMerged(mergedBox, mergedBox, entityBox);
				}
			}
			else if (entity.HasComponent<TransformComponent>())
			{
				// Fallback: Si no tiene mesh/bounding box, usamos solo su posición como un punto
				const auto& transform = entity.GetComponent<TransformComponent>();
				DirectX::BoundingBox pointBox(transform.Position, Math::Vector3(0.5f, 0.5f, 0.5f)); 
            
				if (!hasBounds) { mergedBox = pointBox; hasBounds = true; }
				else { DirectX::BoundingBox::CreateMerged(mergedBox, mergedBox, pointBox); }
			}
		}

		if (!hasBounds) return; // No hay nada enfocable

		// 3. Obtener el centro y el radio de la caja fusionada
		Math::Vector3 center = mergedBox.Center;
		Math::Vector3 extents = mergedBox.Extents;
    
		// El radio de la esfera que envuelve la caja (distancia del centro a una esquina)
		DirectX::XMVECTOR extentsVec = DirectX::XMLoadFloat3(&extents);
		float radius = DirectX::XMVectorGetX(DirectX::XMVector3Length(extentsVec));

		// Si el radio es muy pequeño (ej. un empty object), le damos un tamaño mínimo
		if (radius < 0.1f) radius = 2.0f;

		// 4. Calcular la distancia de la cámara usando trigonometría
		// Distancia = Radio / Seno(FOV / 2). 
		// Nota: Reemplaza GetFov() por tu variable de FOV (usualmente XM_PIDIV4 o 45 grados en radianes)
		float fov = m_camera.GetFieldOfView(); 
		float distance = radius / std::sin(fov * 0.5f);

		// Añadimos un multiplicador (Padding) para que el objeto no roce los bordes de la pantalla (Unity style)
		distance *= 1.5f;

    	// 5. Posicionar la cámara
    	// Obtenemos la posición y el target actuales de la cámara
    	Math::Vector3 currentPos = m_camera.GetPosition();
    	Math::Vector3 currentTarget = m_camera.GetTarget();

    	// Calculamos el vector "Hacia atrás" (Desde el target hacia la cámara)
    	Math::Vector3 backwardDir = currentPos - currentTarget;

    	// Prevención de errores: Si la cámara estaba exactamente dentro del objeto, 
    	// backwardDir sería (0,0,0) y al normalizar daría NaN (explotando todo).
    	if (backwardDir.LengthSquared() < 0.0001f)
    	{
    		// Si no hay dirección válida, forzamos que retroceda en un eje por defecto (ej. hacia -Z)
    		backwardDir = Math::Vector3(0.0f, 0.0f, -1.0f); 
    	}
    	else
    	{
    		backwardDir.Normalize();
    	}

    	// La nueva posición es el centro de la selección + (el vector hacia atrás * la distancia calculada)
    	Math::Vector3 newCameraPosition = center + (backwardDir * distance);

    	// 6. Setear la cámara
    	m_camera.SetPosition(newCameraPosition);
    	m_camera.SetTarget(center);
	}
}
