//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include <SFML/Window/Event.hpp>
#include "Action.hpp"
#include "CommandQueue.hpp"
#include "GameStatus.hpp"
#include "RGBColour.hpp"
#include <map>

class Command;

struct GameRecords
{
	int player_one_throw;
	int player_one_hit;
	int player_two_throw;
	int player_two_hit;
};

class PlayersController
{
public:
	typedef std::unique_ptr<RGBColour> RGBColourPtr;
	PlayersController();
	void HandleEvent(const sf::Event& event, CommandQueue& command_queue);
	void HandleRealTimeInput(CommandQueue& command_queue);

	void HandleControllerInput(CommandQueue& command_queue);

	void AssignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key GetAssignedKey(Action action) const;
	void SetGameStatus(GameStatus status);
	GameStatus GetGameStatus() const;
	void SetPlayersColours(RGBColourPtr colour_one);

	void UpdateColours(CommandQueue& command_queue);

	GameRecords m_game_records;
private:
	void InitialiseActions();
	static bool IsRealTimeAction(Action action);

private:
	std::map<sf::Keyboard::Key, Action> m_key_binding;
	std::map<Action, Command> m_action_binding;

	GameStatus m_current_game_status;

	std::unique_ptr<RGBColour> m_colour_one;

	bool m_should_update_colours;
};

