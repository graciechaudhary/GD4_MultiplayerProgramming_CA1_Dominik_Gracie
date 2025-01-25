#include "MenuState.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"
#include "Button.hpp"

MenuState::MenuState(StateStack& stack, Context context)
    :State(stack, context)
{
    sf::Texture& texture = context.textures->Get(TextureID::kMenuScreen);
    m_background_sprite.setTexture(texture);
    m_background_sprite.setScale(2.f,2.38f);

    auto play_button = std::make_shared<gui::Button>(context);
    play_button->setPosition((context.window->getSize().x - 200)/2, 489);
    play_button->SetText("Play");
    play_button->SetCallback([this]()
    {
        RequestStackPop();
        RequestStackPush(StateID::kGame);
    });

    auto settings_button = std::make_shared<gui::Button>(context);
    settings_button->setPosition((context.window->getSize().x - 200) / 2, 559);
    settings_button->SetText("Settings");
    settings_button->SetCallback([this]()
    {
        RequestStackPush(StateID::kSettings);
    });

    auto exit_button = std::make_shared<gui::Button>(context);
    exit_button->setPosition((context.window->getSize().x - 200) / 2, 629);
    exit_button->SetText("Exit");
    exit_button->SetCallback([this]()
    {
            RequestStackPop();
    });

    m_gui_container.Pack(play_button);
    m_gui_container.Pack(settings_button);
    m_gui_container.Pack(exit_button);

    //Play the music
    //context.music->Play(MusicThemes::kMenuTheme);
    context.music->Play(MusicThemes::kMenu);
}

void MenuState::Draw()
{
    sf::RenderWindow& window = *GetContext().window;
    window.setView(window.getDefaultView());
    window.draw(m_background_sprite);
    window.draw(m_gui_container);
}

bool MenuState::Update(sf::Time dt)
{
    return true;
}

bool MenuState::HandleEvent(const sf::Event& event)
{
    m_gui_container.HandleEvent(event);
    return true;
}

