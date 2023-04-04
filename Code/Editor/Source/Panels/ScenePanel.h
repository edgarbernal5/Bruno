#pragma once

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/form.hpp>

namespace Bruno
{
	class Surface;

	class ScenePanel : public nana::nested_form
	{
	public:
		ScenePanel(nana::window window);
		~ScenePanel() = default;

	private:
		std::unique_ptr<Surface> m_surface;
		int idxx = 0;
		//Scene* m_scene;
	};
}
