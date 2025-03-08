//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once

#include "State.hpp"
#include "Container.hpp"
#include "Button.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include "RGBColour.hpp"
#include "Label.hpp"
#include "ResourceIdentifiers.hpp"

class PreGameState : public State
{
public:
	PreGameState(StateStack& stack, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

private:
	sf::Sprite m_background_sprite;
	gui::Container m_gui_container_one;

	std::vector<gui::Button::Ptr> m_buttons_one;

	std::unique_ptr<RGBColour> m_colour_one;

	bool m_is_player_one_ready;

	sf::Sprite m_sprite_one;

	TextureHolder m_textures;
};

