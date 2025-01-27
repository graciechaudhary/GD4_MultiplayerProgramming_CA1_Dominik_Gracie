#pragma once
#include <SFML/Window/Event.hpp>
#include "Action.hpp"
#include "CommandQueue.hpp"
#include "MissionStatus.hpp"
#include "RGBColour.hpp"
#include <map>

class Command;


class PlayersController
{
public:
	typedef std::unique_ptr<RGBColour> RGBColourPtr;
	PlayersController();
	void HandleEvent(const sf::Event& event, CommandQueue& command_queue);
	void HandleRealTimeInput(CommandQueue& command_queue);

	void AssignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key GetAssignedKey(Action action) const;
	void SetMissionStatus(MissionStatus status);
	MissionStatus GetMissionStatus() const;
	void SetPlayersColours(RGBColourPtr colour_one, RGBColourPtr colour_two);

	void UpdateColours(CommandQueue& command_queue);

private:
	void InitialiseActions();
	static bool IsRealTimeAction(Action action);

private:
	std::map<sf::Keyboard::Key, Action> m_key_binding;
	std::map<Action, Command> m_action_binding;

	MissionStatus m_current_mission_status;

	std::unique_ptr<RGBColour> m_colour_one;
	std::unique_ptr<RGBColour> m_colour_two;

	bool m_should_update_colours;
};

