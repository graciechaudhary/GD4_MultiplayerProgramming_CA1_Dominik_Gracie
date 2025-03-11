//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "Application.hpp"
#include "GameState.hpp"
#include "TitleState.hpp"
#include "MenuState.hpp"
#include "PauseState.hpp"
#include "SettingsState.hpp"
#include "GameOverState.hpp"
#include "PreGameState.hpp"

#include "MultiplayerState.hpp"

const sf::Time Application::kTimePerFrame = sf::seconds(1.f/60.f);

Application::Application() : m_window(sf::VideoMode::getDesktopMode(), "States", sf::Style::Fullscreen)
	, m_stack(State::Context(m_window, m_textures, m_fonts, m_players_controller, m_music, m_sound))
{
	m_window.setKeyRepeatEnabled(false);
	m_fonts.Load(Font::kMain, "Media/Fonts/HappySunshine.otf");
	m_textures.Load(TextureID::kTitleScreen, "Media/Textures/TitleScreen.png");
	
	m_textures.Load(TextureID::kButtonNormal, "Media/Textures/ButtonNormal.png");
	m_textures.Load(TextureID::kButtonSelected, "Media/Textures/ButtonSelected.png");
	m_textures.Load(TextureID::kButtonActivated, "Media/Textures/ButtonPressed.png");
	m_textures.Load(TextureID::kButtons, "Media/Textures/Buttons.png");

	//GracieChaudhary - new menu screen
	m_textures.Load(TextureID::kMenuScreen, "MediaFiles/Textures/Menu/5.png");

	RegisterStates();
	m_stack.PushState(StateID::kTitle);
}

void Application::Run()
{
	sf::Clock clock;
	sf::Time time_since_last_update = sf::Time::Zero;
	while (m_window.isOpen())
	{
		time_since_last_update += clock.restart();
		while(time_since_last_update > kTimePerFrame)
		{
			time_since_last_update -= kTimePerFrame;
			ProcessInput();
			Update(kTimePerFrame);

			if (m_stack.IsEmpty())
			{
				m_window.close();
			}
		}
		Render();

	}
}

void Application::ProcessInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		m_stack.HandleEvent(event);

		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}
	}
}

void Application::Update(sf::Time dt)
{
	m_stack.Update(dt);
}

void Application::Render()
{
	m_window.clear();
	m_stack.Draw();
	m_window.display();
}

void Application::RegisterStates()
{
	m_stack.RegisterState<TitleState>(StateID::kTitle);
	m_stack.RegisterState<MenuState>(StateID::kMenu);
	m_stack.RegisterState<PreGameState>(StateID::kPreGame);
	m_stack.RegisterState<MultiplayerState>(StateID::kHostGame, true);
	m_stack.RegisterState<MultiplayerState>(StateID::kJoinGame, false);
	m_stack.RegisterState<PauseState>(StateID::kPause);
	m_stack.RegisterState<SettingsState>(StateID::kSettings);
	m_stack.RegisterState<GameOverState>(StateID::kGameOver);
}
