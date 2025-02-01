//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "State.hpp"
#include <SFML/Graphics/Text.hpp>
#include "PlayersController.hpp"

class GameOverState : public State
{
public:
	GameOverState(StateStack& stack, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event);

private:
	sf::Text m_game_over_text;
	sf::Text m_player_one_records_text;
	sf::Text m_player_two_records_text;
	sf::Time m_elapsed_time;
	GameRecords m_records;
};

