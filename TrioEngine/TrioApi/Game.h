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

		inline GraphicsDevice* GetGraphicsDevice() { return m_pGraphicsDevice; }
		inline GameServiceContainer& GetServices() { return m_services; }
		inline GameWindow* GetWindow() { return m_pHost->GetWindow(); }

		void Run();
		void RunGame(bool useBlockingRun);
		void Tick();
		virtual void Update(StepTimer const& timer);

	private:
		GameHost*				m_pHost;
		GraphicsDevice*			m_pGraphicsDevice;
		IGraphicsDeviceManager* m_pGraphicsDeviceManager;
		GameServiceContainer	m_services;

		bool m_bDoneFirstUpdate;
		bool m_bDoneFirstDraw;
		bool m_bInRun;

		StepTimer m_timer;

		void CommonConstructor();
		void DrawFrame();

	protected:
		virtual void Initialize();
		virtual void Draw(StepTimer const& timer) {}

		virtual void EndDraw();
	};

}