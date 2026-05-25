#pragma once

#include <optional>

#include "Berta/Controls/Properties/TypedPropertyField.h"
#include <Berta/Controls/Button.h>
#include <Berta/Controls/TextBox.h>
#include <Bruno/Math/Math.h>

namespace Bruno
{
	class PropertyGridFieldVector3 : public Berta::TypedPropertyField<Math::Vector3>
	{
	public:
		PropertyGridFieldVector3(std::string_view label, GetterFn getter, SetterFn setter) :
			TypedPropertyField(label, std::move(getter), std::move(setter))
		{
		}
		virtual void Draw(Berta::Graphics& graphics, const Berta::Rectangle& area, const LayoutConfig& config) override;
		
		virtual void SetFocus() override;
		[[nodiscard]] bool HasFocus() const override;
		
		std::wstring GetValueAsString() const override;

	protected:
		virtual void OnCreate(Berta::Window* parent) override;
		virtual void OnVisibilityChanged(bool visible) override;
		virtual void OnEnableChanged(bool enabled) override;
		
		void SetValueInternal(const Math::Vector3& value) override;
		void SetMixedValuesInternal() override;

		Berta::TextBox m_xyz[3];
	};
}
