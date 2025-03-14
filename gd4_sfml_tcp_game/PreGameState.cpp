//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "PreGameState.hpp"
#include "Command.hpp"
#include "ReceiverCategories.hpp"
#include "Character.hpp"

#include "PlayersController.hpp"

//Dominik Hampejs D00250604
PreGameState::PreGameState(StateStack& stack, Context context) : 
    State(stack, context),
	m_gui_container_one(true),
	m_colour_one(std::make_unique<RGBColour>()),
	m_is_player_one_ready(false),

	m_textures()
{
	sf::Texture& texture = context.textures->Get(TextureID::kMenuScreen);
	m_background_sprite.setTexture(texture);
	m_background_sprite.setScale(2.f, 2.38f);

	m_textures.Load(TextureID::kCharacterMovement, "MediaFiles/Textures/Character/CharacterMovementSheet.png");

	//Set up the sprites of the characters to be coloured
	m_sprite_one.setTexture(m_textures.Get(TextureID::kCharacterMovement));
	m_sprite_one.setPosition(context.window->getSize().x / 2.f - 249, context.window->getSize().y / 2.f - 21);
	m_sprite_one.setTextureRect(sf::IntRect(0, 0, 38, 42));


	//Set up the buttons for the players to select their colours
	std::string color_text = "220";

	auto red_button_one = std::make_shared<gui::Button>(context);
	red_button_one->setPosition(175, 450);
	red_button_one->SetText(color_text);
	red_button_one->SetToggle(true);

	auto green_button_one = std::make_shared<gui::Button>(context);
	green_button_one->setPosition(175, 500);
	green_button_one->SetText(color_text);
	green_button_one->SetToggle(true);

	auto blue_button_one = std::make_shared<gui::Button>(context);
	blue_button_one->setPosition(175, 550);
	blue_button_one->SetText(color_text);
	blue_button_one->SetToggle(true);


	//Set up the ready buttons for the players to confirm their colours
	auto ready_button_one = std::make_shared<gui::Button>(context);
	ready_button_one->setPosition(175, 650);
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


	m_buttons_one.push_back(red_button_one);
	m_buttons_one.push_back(green_button_one);
	m_buttons_one.push_back(blue_button_one);
	m_buttons_one.push_back(ready_button_one);


	//Place the buttons in the container for player one
	for (auto& button : m_buttons_one) {
		m_gui_container_one.Pack(button);
	}
	//Set up the labels for the colours
	auto red_label = std::make_shared<gui::Label>("Red", *context.fonts);
	red_label->setPosition(475, 465);
	red_label->SetColor(sf::Color::Red);
	red_label->SetSize(30);

	auto green_label = std::make_shared<gui::Label>("Green", *context.fonts);
	green_label->setPosition(467, 515);
	green_label->SetColor(sf::Color::Green);
	green_label->SetSize(30);

	auto blue_label = std::make_shared<gui::Label>("Blue", *context.fonts);
	blue_label->setPosition(475, 565);
	blue_label->SetColor(sf::Color::Blue);
	blue_label->SetSize(30);

	m_gui_container_one.Pack(red_label);
	m_gui_container_one.Pack(green_label);
	m_gui_container_one.Pack(blue_label);

	//Set up the labels for the players
	auto player_one_label = std::make_shared<gui::Label>("Player One", *context.fonts);
	player_one_label->setPosition(224, 305);
	player_one_label->SetColor(sf::Color::Red);
	player_one_label->SetSize(30);

	auto player_two_label = std::make_shared<gui::Label>("Player Two", *context.fonts);
	player_two_label->setPosition(674, 305);
	player_two_label->SetColor(sf::Color::Red);
	player_two_label->SetSize(30);

	m_gui_container_one.Pack(player_one_label);

	//Set up the title label
	auto title_label = std::make_shared<gui::Label>("Colour Selection", *context.fonts);
	title_label->setPosition(310, 100);
	title_label->SetColor(sf::Color::Red);
	title_label->SetSize(80);

	m_gui_container_one.Pack(title_label);

}
//Dominik Hampejs D00250604
void PreGameState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.setView(window.getDefaultView());
	//window.draw(m_background_sprite);
	window.draw(m_gui_container_one);
	window.draw(m_sprite_one);

}
//Dominik Hampejs D00250604
bool PreGameState::Update(sf::Time dt)
{
	//If both players are ready, set the colours and move to the game state
	if (m_is_player_one_ready) { 

		GetContext().players_controller->SetPlayersColours(std::move(m_colour_one));

		RequestStackPop();
	}
    return false;
}

//Dominik Hampejs D00250604
bool PreGameState::HandleEvent(const sf::Event& event)
{
	bool is_one_colour_selecting = false;

	//If the players are not ready, allow them to select their colours
	if (!(m_is_player_one_ready))
	{
		//If the escape key is pressed, go to the pause state
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		{
			RequestStackPush(StateID::kPause);
		}

		//Check if the players are selecting their colours
		for (int i = 0; i < 3; i++)
		{
			is_one_colour_selecting = m_buttons_one[i]->IsActive();

			//If the player one is selecting their colour, allow them to change it
			if (is_one_colour_selecting)
			{
				if (event.type == sf::Event::KeyPressed) {
					//Pressing W or S will deactivate the button
					if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::S)
					{
						m_buttons_one[i]->Deactivate();
					}
					int add = 0;
					//Pressing D or A will change the colour
					if (event.key.code == sf::Keyboard::D)
					{
						add = 20;
					}
					else if (event.key.code == sf::Keyboard::A)
					{
						add = -20;
					}
					//Change the colour based on the button pressed
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
				m_sprite_one.setColor(m_colour_one->GetColour()); //Set the colour of the sprite
			}
		}

		m_gui_container_one.HandleEvent(event);
	}
    return false;
}
