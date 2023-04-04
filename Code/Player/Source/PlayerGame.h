#pragma once

#include <Bruno.h>
#include <Bruno/Platform/DirectX/Surface.h>

namespace Bruno {

	class PlayerGame : public Game
	{
	public:
		PlayerGame(const GameParameters& parameters);

	protected:
		void DoOnInitialize() override;
		void Tick() override;
		void OnClientSizeChanged() override;

	private:
		std::unique_ptr<Surface> m_surface;
	};

}