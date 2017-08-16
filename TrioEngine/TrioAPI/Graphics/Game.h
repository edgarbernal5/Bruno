#pragma once

#include "TrioAPI.h"

#include "GameHost.h"
#include "GameServiceContainer.h"

#include "StepTimer.h"


namespace Cuado
{
	class TRIOAPI_DLL GameHost;
	class TRIOAPI_DLL GraphicsDevice;
	class TRIOAPI_DLL IGraphicsDeviceManager;

	class TRIOAPI_DLL Game {
	public:
		Game();
		Game(HINSTANCE hInstance);
		
		virtual ~Game();

		inline GraphicsDevice* GetGraphicsDevice()	{ return m_graphicsDevice; }
		inline GameServiceContainer& GetServices()	{ return m_services; }
		inline GameWindow* GetWindow()				{ return m_host->GetWindow(); }
		
		void Run();
		void RunGame(bool useBlockingRun);
		void Tick();
		virtual void Update(DX::StepTimer const& timer);

		friend class WindowsGameWindow;
	private:
		HINSTANCE				m_hInstance;
		GameHost*				m_host;
		GraphicsDevice*			m_graphicsDevice;
		GameServiceContainer	m_services;

		IGraphicsDeviceManager* m_graphicsDeviceManager;

		bool m_doneFirstUpdate;
		bool m_doneFirstDraw;
		bool m_inRun;
		DX::StepTimer m_timer;

		void CommonConstructor();
		void DrawFrame();
		
	protected:
		virtual void Initialize();
		virtual void Draw(DX::StepTimer const& timer) {}

		virtual void EndDraw();
	};
}