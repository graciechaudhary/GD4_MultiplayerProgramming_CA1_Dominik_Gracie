////Gracie Chaudhary D00251769  
////Dominik Hampejs D00250604  
//#include "GameState.hpp"
//#include "PlayersController.hpp"
//#include "GameStatus.hpp"
//
//GameState::GameState(StateStack& stack, Context context) : State(stack, context), m_world(*context.window, *context.fonts, *context.sounds), m_players_controller(*context.players_controller)
//{
//	//Play the music
//
//		
//	context.music->Play(MusicThemes::kGame);
//	
//	HandleEvent(sf::Event());
//}
//
//void GameState::Draw()
//{
//	m_world.Draw();
//}
//
//bool GameState::Update(sf::Time dt)
//{
//	m_world.Update(dt);
//	//If one of the players is dead, then the other player wins
//	if (!m_world.HasAlivePlayerOne())
//	{
//		m_players_controller.m_game_records = m_world.GetGameRecords();
//		m_players_controller.SetGameStatus(GameStatus::kPlayerTwoWin);
//		RequestStackPush(StateID::kGameOver);
//	}
//	//else if(!m_world.HasAlivePlayerTwo())
//	//{ 
//	//	m_players_controller.m_game_records = m_world.GetGameRecords();
//	//	m_players_controller.SetGameStatus(GameStatus::kPlayerOneWin);
//	//	RequestStackPush(StateID::kGameOver);
//	//}
//	CommandQueue& commands = m_world.GetCommandQueue();
//	m_players_controller.HandleRealTimeInput(commands);
//	//m_players_controller.HandleControllerInput(commands);
//	m_players_controller.UpdateColours(commands);
//	return true;
//}
//
//bool GameState::HandleEvent(const sf::Event& event)
//{
//	CommandQueue& commands = m_world.GetCommandQueue();
//	m_players_controller.HandleEvent(event, commands);
//
//	//Escape should bring up the pause menu
//	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
//	{
//		RequestStackPush(StateID::kPause);
//	}
//	return true;
//}
