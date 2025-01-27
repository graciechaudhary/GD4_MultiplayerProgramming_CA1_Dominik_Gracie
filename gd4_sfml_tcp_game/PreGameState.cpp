#include "PreGameState.hpp"

PreGameState::PreGameState(StateStack& stack, Context context) : 
    State(stack, context),
	m_gui_container_one(true),
	m_gui_container_two(false),
	m_colour_one(std::make_unique<RGBColour>(0, 0, 0)),
	m_colour_two(std::make_unique<RGBColour>(0, 0, 0)),
	m_is_one_colour_selecting(false),
	m_is_two_colour_selecting(false),
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
			m_buttons_one[3]->SetText("Ready");
			m_is_player_one_ready = true;
		});

	auto ready_button_two = std::make_shared<gui::Button>(context);
	ready_button_two->setPosition(400, 500);
	ready_button_two->SetText("Confirm");
	ready_button_two->SetCallback([this]()
		{
			m_buttons_two[3]->SetText("Ready");
			m_is_player_two_ready = true;
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
		RequestStackPop();
		RequestStackPush(StateID::kGame);
	}
    return true;
}

bool PreGameState::HandleEvent(const sf::Event& event)
{
	if (!(m_is_player_one_ready && m_is_player_two_ready))
	{
		m_gui_container_one.HandleEvent(event);
		m_gui_container_two.HandleEvent(event);
	}
    return false;
}
