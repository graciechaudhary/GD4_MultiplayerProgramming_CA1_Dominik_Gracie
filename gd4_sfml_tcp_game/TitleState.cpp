//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "TitleState.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include "ResourceHolder.hpp"
#include "Utility.hpp"

TitleState::TitleState(StateStack& stack, Context context) : State(stack, context), m_show_text(true), m_text_effect_time(sf::Time::Zero)
{
    sf::Texture& texture = context.textures->Get(TextureID::kMenuScreen);
    m_background_sprite.setTexture(texture);
    m_background_sprite.setScale(2.f, 2.38f);

    m_text.setFont(context.fonts->Get(Font::kMain));
    m_text.setString("Press any key to continue");
    m_text.setCharacterSize(50);
    m_text.setOutlineColor(sf::Color::Red);
    m_text.setOutlineThickness(2.5f);
    Utility::CentreOrigin(m_text);
    m_text.setPosition(context.window->getView().getSize().x/2.f,context.window->getView().getSize().y * 0.9f);

    m_game_name.setFont(context.fonts->Get(Font::kMain));
    m_game_name.setString("Snowballed");
    m_game_name.setCharacterSize(150);
    m_game_name.setOutlineColor(sf::Color::Red);
    m_game_name.setOutlineThickness(4.f);
    m_game_name.setLetterSpacing(1.5f);
    Utility::CentreOrigin(m_game_name);
    m_game_name.setPosition(context.window->getView().getSize().x / 2.f, context.window->getView().getSize().y * 0.35f);
}


void TitleState::Draw()
{
    sf::RenderWindow& window = *GetContext().window;
    window.draw(m_background_sprite);
    window.draw(m_game_name);

    if (m_show_text)
    {
        window.draw(m_text);
    }
}

bool TitleState::Update(sf::Time dt)
{
    m_text_effect_time += dt;
    if (m_text_effect_time >= sf::seconds(0.5))
    {
        m_show_text = !m_show_text;
        m_text_effect_time = sf::Time::Zero;
    }
    return false;
}

bool TitleState::HandleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::KeyReleased)
    {
        RequestStackPop();
        RequestStackPush(StateID::kMenu);
    }
    return false;
}
