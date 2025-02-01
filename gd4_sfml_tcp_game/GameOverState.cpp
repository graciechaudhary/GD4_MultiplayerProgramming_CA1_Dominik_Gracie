//Grie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "GameOverState.hpp"
#include "ResourceHolder.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include "PlayersController.hpp"
#include "Utility.hpp"
#include <iostream>
#include <sstream>


GameOverState::GameOverState(StateStack& stack, Context context)
    : State(stack, context)
    , m_game_over_text()
    , m_elapsed_time(sf::Time::Zero)
{
    sf::Font& font = context.fonts->Get(Font::kMain);
    sf::Vector2f window_size(context.window->getSize());

    m_records = context.players_controller->m_game_records;

    m_game_over_text.setFont(font);
    switch (context.players_controller->GetGameStatus()) {
    case GameStatus::kPlayerOneWin:
        m_game_over_text.setString("Player one wins");
        break;
    case GameStatus::kPlayerTwoWin:
        m_game_over_text.setString("Player two wins");
        break;
    }

    m_game_over_text.setCharacterSize(90);
    Utility::CentreOrigin(m_game_over_text);
    m_game_over_text.setPosition(0.49f * window_size.x, 0.33 * window_size.y);


    m_player_one_records_text.setFont(font);
    std::ostringstream oss; 
    oss << "Player One Throws:\t" << m_records.player_one_throw << "\n";
    oss << "Player One Hits:\t" << m_records.player_one_hit << "\n";
    oss << "Player One Misses:\t" << m_records.player_one_throw - m_records.player_one_hit  << "\n";
    oss << "Player One Accuracy:\t" << (m_records.player_one_throw == 0 ? 0 : ((float)m_records.player_one_hit / (float)m_records.player_one_throw) * 100)<< "%\n";
   
    m_player_one_records_text.setString(oss.str());
    m_player_one_records_text.setCharacterSize(40);
    Utility::CentreOrigin(m_player_one_records_text);
    m_player_one_records_text.setPosition(0.27f * window_size.x, 0.63 * window_size.y);

    m_player_two_records_text.setFont(font);
    std::ostringstream os;
    os << "Player Two Throws:\t" << m_records.player_two_throw << "\n";
    os << "Player Two Hits:\t" << m_records.player_two_hit << "\n";
    os << "Player Two Misses:\t" << m_records.player_two_throw - m_records.player_two_hit << "\n";
    os << "Player Two Accuracy:\t" << (m_records.player_two_throw == 0 ? 0 : ((float)m_records.player_two_hit / (float)m_records.player_two_throw) * 100) << "%\n";

    m_player_two_records_text.setString(os.str());
    m_player_two_records_text.setCharacterSize(40);
    Utility::CentreOrigin(m_player_two_records_text);
    m_player_two_records_text.setPosition(0.74f * window_size.x, 0.63 * window_size.y);
   
    context.music->Play(MusicThemes::kGameOver);
}

void GameOverState::Draw()
{
    sf::RenderWindow& window = *GetContext().window;
    window.setView(window.getDefaultView());

    //Create a dark semi-transparent background
    sf::RectangleShape background_shape;
    background_shape.setFillColor(sf::Color(0, 0, 0, 150));
    background_shape.setSize(window.getView().getSize());

    window.draw(background_shape);
    window.draw(m_game_over_text);
    window.draw(m_player_one_records_text);
    window.draw(m_player_two_records_text);
}

bool GameOverState::Update(sf::Time dt)
{
    return false;
}

bool GameOverState::HandleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::KeyReleased)
    {
        RequestStackClear();
        RequestStackPush(StateID::kMenu);
    }
    return false;
}
