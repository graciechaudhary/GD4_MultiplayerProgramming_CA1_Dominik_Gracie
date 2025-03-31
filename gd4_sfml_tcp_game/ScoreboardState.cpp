#include "ScoreboardState.hpp"
#include "Utility.hpp"
#include "PlayersController.hpp"
#include <sstream>

ScoreboardState::ScoreboardState(StateStack& stack, Context context) : State(stack,context),
    m_background_sprite(),
    m_instruction_text(),
    m_scoreboard()
{
    sf::Texture& texture = context.textures->Get(TextureID::kScoreboardScreen);
    m_background_sprite.setTexture(texture);
    m_background_sprite.setScale(static_cast<float>(context.window->getSize().x) / texture.getSize().x, static_cast<float>(context.window->getSize().y) / texture.getSize().y);

    m_results = context.players_controller->m_score_ss.str();
	context.players_controller->m_score_ss.str(std::string());

    sf::Font& font = context.fonts->Get(Font::kMain);
    sf::Vector2f view_size = context.window->getView().getSize();

    m_scoreboard.setFont(font);
    m_scoreboard.setString(m_results);
    m_scoreboard.setCharacterSize(40);
    Utility::CentreOrigin(m_scoreboard);
    m_scoreboard.setPosition(0.5f * view_size.x, 0.1f * view_size.y);

    m_instruction_text.setFont(font);
    m_instruction_text.setString("Press Enter to return to main menu");
	m_instruction_text.setOutlineColor(sf::Color::Red);
    m_instruction_text.setOutlineThickness(2.f);
    m_instruction_text.setCharacterSize(40);
    Utility::CentreOrigin(m_instruction_text);
    m_instruction_text.setPosition(0.5f * view_size.x, 0.95f * view_size.y);

	m_player_list.setFont(font);
	m_player_list.setString("Players this Game");
    m_player_list.setOutlineColor(sf::Color::Red);
    m_player_list.setOutlineThickness(3.f);
	m_player_list.setCharacterSize(50);
	Utility::CentreOrigin(m_player_list);
    //setting on top middle
	m_player_list.setPosition(0.5f * view_size.x, 0.045f * view_size.y);

	m_top_kills.setFont(font);
	m_top_kills.setString("Top Kills");
	m_top_kills.setOutlineColor(sf::Color::Red);
	m_top_kills.setOutlineThickness(3.f);
	m_top_kills.setCharacterSize(50);
	Utility::CentreOrigin(m_top_kills);
    //setting left top
	m_top_kills.setPosition(0.15f * view_size.x, 0.17f * view_size.y);

	m_top_time.setFont(font);
	m_top_time.setString("Top Time");
	m_top_time.setOutlineColor(sf::Color::Red);
	m_top_time.setOutlineThickness(3.f);
	m_top_time.setCharacterSize(50);
	Utility::CentreOrigin(m_top_time);
	//setting right top
    m_top_time.setPosition(0.85f * view_size.x, 0.17f * view_size.y);
}

void ScoreboardState::Draw()
{
    sf::RenderWindow& window = *GetContext().window;
    window.setView(window.getDefaultView());

    sf::RectangleShape backgroundShape;
    backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
    backgroundShape.setSize(window.getView().getSize());

    window.draw(backgroundShape);
    window.draw(m_scoreboard);
    window.draw(m_background_sprite);
   
	window.draw(m_player_list);
    window.draw(m_top_kills);
    window.draw(m_top_time);

	//blinking effect for instruction where the text will be displayed every 0.25 seconds
    

    if (m_show_text) {
        window.draw(m_instruction_text);
    }

}

bool ScoreboardState::Update(sf::Time dt)
{
	m_blink_timer += dt;
	if (m_blink_timer >= sf::seconds(0.5))
	{
		m_show_text = !m_show_text;
		m_blink_timer = sf::Time::Zero;
	}
    return false;
    
}

bool ScoreboardState::HandleEvent(const sf::Event& event)
{
    if (event.key.code == sf::Keyboard::Enter)
    {
        RequestStackClear();
        RequestStackPush(StateID::kMenu);
    }
	return false;
}
