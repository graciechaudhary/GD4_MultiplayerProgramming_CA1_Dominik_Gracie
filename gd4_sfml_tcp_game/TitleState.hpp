//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "State.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>


class TitleState : public State
{
public:
	TitleState(StateStack&, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

private:
	sf::Sprite m_background_sprite;
	sf::Text m_text;

	bool m_show_text;
	sf::Time m_text_effect_time;
	sf::Text m_game_name;
};

