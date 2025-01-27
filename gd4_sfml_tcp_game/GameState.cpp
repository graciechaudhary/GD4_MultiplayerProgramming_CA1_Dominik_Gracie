#include "GameState.hpp"
#include "PlayersController.hpp"
#include "MissionStatus.hpp"

GameState::GameState(StateStack& stack, Context context) : State(stack, context), m_world(*context.window, *context.fonts, *context.sounds), m_players_controller(*context.players_controller)
{
	//Play the music
	context.music->Play(MusicThemes::kMissionTheme);
	HandleEvent(sf::Event());
}

void GameState::Draw()
{
	m_world.Draw();
}

bool GameState::Update(sf::Time dt)
{
	m_world.Update(dt);
	if (!m_world.HasAlivePlayer())
	{
		m_players_controller.SetMissionStatus(MissionStatus::kMissionFailure);
		RequestStackPush(StateID::kGameOver);
	}
	else if(m_world.HasPlayerReachedEnd())
	{ 
		m_players_controller.SetMissionStatus(MissionStatus::kMissionSuccess);
		RequestStackPush(StateID::kGameOver);
	}
	CommandQueue& commands = m_world.GetCommandQueue();
	m_players_controller.HandleRealTimeInput(commands);
	m_players_controller.UpdateColours(commands);
	return true;
}

bool GameState::HandleEvent(const sf::Event& event)
{
	CommandQueue& commands = m_world.GetCommandQueue();
	m_players_controller.HandleEvent(event, commands);

	//Escape should bring up the pause menu
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
		RequestStackPush(StateID::kPause);
	}
	return true;
}
