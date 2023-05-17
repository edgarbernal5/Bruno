#pragma once

#include "Bruno/Core/Base.h"

#include <cstdint>
#include <string>
#include <memory>

namespace Bruno
{
	struct GameParameters
	{
		std::string Name = "Bruno";
		uint32_t WindowWidth = 800;
		uint32_t WindowHeight = 600;
	};

	class GameWindow;
	class GraphicsDevice;

	class Game
	{
	public:
		Game(const GameParameters& parameters);
		virtual ~Game();

		inline GraphicsDevice* GetDevice() { return m_device.get(); }
		void Run();

		static Game* GetInstance() { return g_instance; }
		
		friend class WindowsGameWindow;
	protected:
		GameWindow* m_gameWindow;
		GameParameters m_parameters;
		std::shared_ptr<GraphicsDevice> m_device;

		virtual void DoOnInitialize() = 0;
		virtual void OnTick();
		virtual void OnClientSizeChanged();

	private:
		static Game* g_instance;

	};

	Game* CreateGame(int argc, char** argv);
}
