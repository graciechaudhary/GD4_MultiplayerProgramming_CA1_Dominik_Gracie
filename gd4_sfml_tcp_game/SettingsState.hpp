//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "State.hpp"
#include "PlayersController.hpp"
#include "Container.hpp"
#include "Button.hpp"
#include "Label.hpp"
#include <SFML/Graphics/Sprite.hpp>

#include <array>


class SettingsState : public State
{
public:
	SettingsState(StateStack& stack, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event);

private:
	void UpdateLabels();
	void AddButtonLabel(Action action, float y, const std::string& text, Context context);

private:
	sf::Sprite m_background_sprite;
	gui::Container m_gui_container;
	std::array<gui::Button::Ptr, static_cast<int>(Action::kActionCount)> m_binding_buttons;
	std::array<gui::Label::Ptr, static_cast<int>(Action::kActionCount)> m_binding_labels;
};

