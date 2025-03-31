//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "State.hpp"
#include "MenuOptions.hpp"
#include "Container.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <vector>


class MenuState : public State
{
public:
	MenuState(StateStack& stack, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

private: 

	void SetupInstructionsVisuals(Context context);

private:
	sf::Sprite m_background_sprite;
	gui::Container m_gui_container;
	sf::Text m_game_name;
	sf::Sprite m_WSAD_sprite;
	sf::Sprite m_Space_sprite;
	sf::Text m_WSAD_text;
	sf::Text m_Space_text;
};

