#pragma once

#include "TrioApiDecl.h"

#include "GameHost.h"
#include "GameServiceContainer.h"
#include "StepTimer.h"

namespace TrioEngine
{
	class GameHost;
	class GraphicsDevice;
	class IGraphicsDeviceManager;

	class TRIOAPI_DECL Game
	{
	public:
		Game();
		virtual ~Game();

		inline GraphicsDevice* GetGraphicsDevice() { return m_graphicsDevice; }
		inline GameServiceContainer& GetServices() { return m_services; }
		inline GameWindow* GetWindow() { return m_host->GetWindow(); }

		void Run();
		void RunGame(bool useBlockingRun);
		void Tick();
		virtual void Update(StepTimer const& timer);

	private:
		GameHost*				m_host;
		GraphicsDevice*			m_graphicsDevice;
		IGraphicsDeviceManager* m_graphicsDeviceManager;
		GameServiceContainer	m_services;

		bool m_doneFirstUpdate;
		bool m_doneFirstDraw;
		bool m_inRun;

		StepTimer m_timer;

		void CommonConstructor();
		void DrawFrame();

	protected:
		virtual void Initialize();
		virtual void Draw(StepTimer const& timer) {}

		virtual void EndDraw();
	};

}