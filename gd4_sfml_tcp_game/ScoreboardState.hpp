#pragma once

#include "State.hpp"
#include <SFML/Graphics.hpp>

class ScoreboardState :
    public State
{
public:
	ScoreboardState(StateStack& stack, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

private:
	sf::Sprite m_background_sprite;
	sf::Text m_scoreboard;
	sf::Text m_instruction_text;
	std::string m_results;
};

