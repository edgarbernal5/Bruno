#pragma once

#include "Bruno/Core/Base.h"
#include "Bruno/Core/Application.h"
#include "Bruno/Core/GameTimer.h"
#include "Bruno/Renderer/ShaderCache.h"
#include "Bruno/Renderer/PipelineStateObjectCache.h"
#include "Bruno/Content/ContentManager.h"

#include "Bruno/Core/KeyCodes.h"

#include <cstdint>
#include <string>
#include <memory>

namespace Bruno
{
	class GraphicsDevice;
	class AbstractAssetManager;

	class Game : public Application
	{
	public:
		Game(const ApplicationParameters& parameters);
		virtual ~Game();

		inline GraphicsDevice* GetDevice() { return m_device.get(); }
		static Game* GetInstance() { return (Game*)g_instance; }
		inline ShaderCache& GetShaderCache() { return m_shaderCache; }
		inline PipelineStateObjectCache& GetPsoCache() { return m_psoCache; }
		inline AbstractAssetManager* GetAssetManager() { return m_assetManager.get(); }
		virtual void OnTick();

		friend class WindowsGameWindow;
		friend class NanaWindow;
	protected:
		virtual void OnInitialize() override;
		virtual void OnInitializeWindow(const WindowParameters& windowParameters) = 0;
		virtual void OnRun() override;

		virtual void OnGameLoop(const GameTimer& timer) = 0;

		virtual void OnActivated() override;
		virtual void OnDeactivated() override;
		virtual void OnResizeMoveStarted() override;
		virtual void OnResizeMoveFinished() override;

		GameTimer m_timer;
		ContentManager m_contentManager;
		std::shared_ptr<GraphicsDevice> m_device;
		std::shared_ptr<AbstractAssetManager> m_assetManager;
		ShaderCache m_shaderCache;
		PipelineStateObjectCache m_psoCache;

		int m_framesThisSecond = 0;
		int m_framesPerSecond = 0;
		float m_elapsedTime = 0.0f;

	private:
		bool m_gamePaused = false;
	};

}

namespace Bruno::Graphics
{
	inline ShaderCache& GetShaderCache()
	{
		return Game::GetInstance()->GetShaderCache();
	}

	inline PipelineStateObjectCache& GetPsoCache()
	{
		return Game::GetInstance()->GetPsoCache();
	}
}