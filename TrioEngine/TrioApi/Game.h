#pragma once

#include "TrioApiRequisites.h"

#include "GameHost.h"
#include "GameServiceContainer.h"
#include "StepTimer.h"
#include "Scene.h"


namespace TrioEngine
{
	class GameHost;
	class GraphicsDevice;
	class IGraphicsDeviceManager;
	class RenderPath;

	class TRIO_API_EXPORT Game
	{
	public:
		Game();
		virtual ~Game();

		inline GraphicsDevice* GetGraphicsDevice() { return m_graphicsDevice; }
		inline GameServiceContainer& GetServices() { return m_services; }
		inline GameWindow* GetWindow() { return m_host->GetWindow(); }
		inline Scene* GetScene() { return m_scene; }

		void Run();
		void RunGame(bool useBlockingRun);
		void Tick();
		virtual void Update(StepTimer const& timer);

	private:
		GameHost*				m_host;
		GraphicsDevice*			m_graphicsDevice;
		IGraphicsDeviceManager* m_graphicsDeviceManager;
		GameServiceContainer	m_services;
		Scene*					m_scene;
		RenderPath*				m_renderPath;

		bool m_doneFirstUpdate;
		bool m_doneFirstDraw;
		bool m_isRunning;

		StepTimer m_timer;

		void CommonConstructor();
		void DrawFrame();

	protected:
		virtual void Initialize();
		virtual void Draw(StepTimer const& timer);

		virtual void EndDraw();
	};

}