//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "State.hpp"
#include "World.hpp"

class GameState : public State
{
public:
	GameState(StateStack& stack, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

private:
	World m_world;
	PlayersController& m_players_controller;
	sf::Time m_time_till_end;
	sf::Time m_time_counter;
};

