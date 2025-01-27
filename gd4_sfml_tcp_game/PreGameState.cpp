#include "PreGameState.hpp"
#include "Command.hpp"
#include "ReceiverCategories.hpp"
#include "Character.hpp"

#include "PlayersController.hpp"

PreGameState::PreGameState(StateStack& stack, Context context) : 
    State(stack, context),
	m_gui_container_one(true),
	m_gui_container_two(false),
	m_colour_one(std::make_unique<RGBColour>(0, 0, 0)),
	m_colour_two(std::make_unique<RGBColour>(0, 0, 0)),
	m_is_player_one_ready(false),
	m_is_player_two_ready(false)
{
	m_background_sprite.setTexture(context.textures->Get(TextureID::kTitleScreen));


	auto red_button_one = std::make_shared<gui::Button>(context);
	red_button_one->setPosition(100, 250);
	red_button_one->SetText("Red");
	red_button_one->SetToggle(true);

	auto green_button_one = std::make_shared<gui::Button>(context);
	green_button_one->setPosition(100, 300);
	green_button_one->SetText("Green");
	green_button_one->SetToggle(true);

	auto blue_button_one = std::make_shared<gui::Button>(context);
	blue_button_one->setPosition(100, 350);
	blue_button_one->SetText("Blue");
	blue_button_one->SetToggle(true);

	auto red_button_two = std::make_shared<gui::Button>(context);
	red_button_two->setPosition(400, 250);
	red_button_two->SetText("Red");
	red_button_two->SetToggle(true);

	auto green_button_two = std::make_shared<gui::Button>(context);
	green_button_two->setPosition(400, 300);
	green_button_two->SetText("Green");
	green_button_two->SetToggle(true);

	auto blue_button_two = std::make_shared<gui::Button>(context);
	blue_button_two->setPosition(400, 350);
	blue_button_two->SetText("Blue");
	blue_button_two->SetToggle(true);

	auto ready_button_one = std::make_shared<gui::Button>(context);
	ready_button_one->setPosition(100, 500);
	ready_button_one->SetText("Confirm");
	ready_button_one->SetCallback([this]()
		{
			std::string text;
			if (m_is_player_one_ready)
			{
				m_is_player_one_ready = false;
				text = "Confirm";
			}
			else {
				text = "Ready";
				m_is_player_one_ready = true;
			}
			m_buttons_one[3]->SetText(text);
		});

	auto ready_button_two = std::make_shared<gui::Button>(context);
	ready_button_two->setPosition(400, 500);
	ready_button_two->SetText("Confirm");
	ready_button_two->SetCallback([this]()
		{
			std::string text;
			if (m_is_player_two_ready)
			{
				m_is_player_two_ready = false;
				text = "Confirm";
			}
			else {
				text = "Ready";
				m_is_player_two_ready = true;
			}
			m_buttons_two[3]->SetText(text);

		});

	m_buttons_one.push_back(red_button_one);
	m_buttons_one.push_back(green_button_one);
	m_buttons_one.push_back(blue_button_one);
	m_buttons_one.push_back(ready_button_one);

	m_buttons_two.push_back(red_button_two);
	m_buttons_two.push_back(green_button_two);
	m_buttons_two.push_back(blue_button_two);
	m_buttons_two.push_back(ready_button_two);

	for (auto& button : m_buttons_one) {
		m_gui_container_one.Pack(button);
	}

	for (auto& button : m_buttons_two) {
		m_gui_container_two.Pack(button);
	}
}

void PreGameState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.setView(window.getDefaultView());
	window.draw(m_background_sprite);
	window.draw(m_gui_container_one);
	window.draw(m_gui_container_two);
}

bool PreGameState::Update(sf::Time dt)
{
	if (m_is_player_one_ready && m_is_player_two_ready) {

		GetContext().players_controller->SetPlayersColours(std::move(m_colour_one), std::move(m_colour_two));

		RequestStackPop();
	}
    return false;
}

bool PreGameState::HandleEvent(const sf::Event& event)
{
	bool is_one_colour_selecting = false;
	bool is_two_colour_selecting = false;

	if (!(m_is_player_one_ready && m_is_player_two_ready))
	{
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		{
			RequestStackPush(StateID::kPause);
		}

		
		for (int i = 0; i < 3; i++)
		{
			is_one_colour_selecting = m_buttons_one[i]->IsActive();
			is_two_colour_selecting = m_buttons_two[i]->IsActive();

			if (is_one_colour_selecting)
			{
				if (event.type == sf::Event::KeyPressed) {
					if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::S)
					{
						m_buttons_one[i]->Deactivate();
					}
					int add = 0;
					if (event.key.code == sf::Keyboard::D)
					{
						add = 1;
					}
					else if (event.key.code == sf::Keyboard::A)
					{
						add = -1;
					}
					switch (i)
					{
					case 0:
						m_colour_one->addRed(add);
						m_buttons_one[i]->SetText(std::to_string(m_colour_one->GetRed()));
						break;
					case 1:
						m_colour_one->addGreen(add);
						m_buttons_one[i]->SetText(std::to_string(m_colour_one->GetGreen()));
						break;
					case 2:
						m_colour_one->addBlue(add);
						m_buttons_one[i]->SetText(std::to_string(m_colour_one->GetBlue()));
						break;
					default:
						break;
					}
				}
			}

			if (is_two_colour_selecting)
			{
				if (event.type == sf::Event::KeyPressed) {
					if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::Up)
					{
						m_buttons_two[i]->Deactivate();
					}
					int add = 0;
					if (event.key.code == sf::Keyboard::Right)
					{
						add = 1;
					}
					else if (event.key.code == sf::Keyboard::Left)
					{
						add = -1;
					}
					switch (i)
					{
					case 0:
						m_colour_two->addRed(add);
						m_buttons_two[i]->SetText(std::to_string(m_colour_two->GetRed()));
						break;
					case 1:
						m_colour_two->addGreen(add);
						m_buttons_two[i]->SetText(std::to_string(m_colour_two->GetGreen()));
						break;
					case 2:
						m_colour_two->addBlue(add);
						m_buttons_two[i]->SetText(std::to_string(m_colour_two->GetBlue()));
						break;
					default:
						break;
					}
				}
			}
			else
			{

			}
		}
		m_gui_container_one.HandleEvent(event);
		m_gui_container_two.HandleEvent(event);




	}
    return false;
}
