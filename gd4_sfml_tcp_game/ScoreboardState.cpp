#include "ScoreboardState.hpp"
#include "Utility.hpp"
#include "PlayersController.hpp"
#include <sstream>

ScoreboardState::ScoreboardState(StateStack& stack, Context context) : State(stack,context),
    m_background_sprite(),
    m_instruction_text(),
    m_scoreboard()
{
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
    m_instruction_text.setCharacterSize(40);
    Utility::CentreOrigin(m_instruction_text);
    m_instruction_text.setPosition(0.48f * view_size.x, 0.9f * view_size.y);
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
    window.draw(m_instruction_text);
}

bool ScoreboardState::Update(sf::Time dt)
{
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
