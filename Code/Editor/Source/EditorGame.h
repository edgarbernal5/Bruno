#pragma once

#include <Bruno.h>

#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/place.hpp>

namespace Bruno
{
	class NanaGameWindow;
	class GraphicsDevice;

	class EditorGame : public Game
	{
	public:
		EditorGame(const GameParameters& parameters);
		~EditorGame();

	protected:
		void InitUI();
		virtual void DoOnInitialize() override;
		void DoOnUpdate(const GameTimer& timer) override;
		void DoOnDraw() override;

	private:
		NanaGameWindow* m_nanaGameWindow;
		nana::menubar m_menubar;
		nana::place m_place;
	};
}