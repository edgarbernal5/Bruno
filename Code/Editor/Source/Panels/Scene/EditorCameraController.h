#pragma once
#include "SelectionService.h"

namespace Bruno
{
    class Camera;
    class SelectionService;
    class Scene;
    
    class EditorCameraController
    {
    public:
        EditorCameraController(Camera& camera, Scene* scene, SelectionService* selectionService);
        
        void FocusOnSelection();
    private:
        Camera& m_camera;
        SelectionService* m_selectionService;
        Scene* m_scene;
    };
}
