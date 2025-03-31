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
	void SetupText(Context context);

private:
	sf::Sprite m_background_sprite;
	sf::Text m_scoreboard;
	sf::Text m_instruction_text;
	sf::Text m_top_kills;
	sf::Text m_top_time;
	sf::Text m_player_list;
	std::string m_results;

	sf::Time m_blink_timer;
	bool m_show_text = true;
};

