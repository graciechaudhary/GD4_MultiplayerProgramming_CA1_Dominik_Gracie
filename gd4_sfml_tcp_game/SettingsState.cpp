//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "SettingsState.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"

SettingsState::SettingsState(StateStack& stack, Context context)
	: State(stack, context)
	, m_gui_container(true)
{
	m_background_sprite.setTexture(context.textures->Get(TextureID::kMenuScreen));
	m_background_sprite.setScale(2.f, 2.38f);

	//Build key binding buttons and labels
	AddButtonLabel(Action::kMoveUp, 100.f, "Move Up", context);
	AddButtonLabel(Action::kMoveDown, 150.f, "Move Down", context);
	AddButtonLabel(Action::kMoveRight, 200.f, "Move Right", context);
	AddButtonLabel(Action::kMoveLeft, 250.f, "Move Left", context);
	AddButtonLabel(Action::kThrow, 300.f, "Throw", context);


	UpdateLabels();

	auto back_button = std::make_shared<gui::Button>(context);
	back_button->setPosition(390.f, 620.f);
	back_button->SetText("Back");
	back_button->SetCallback(std::bind(&SettingsState::RequestStackPop, this));
	m_gui_container.Pack(back_button);
}

void SettingsState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.draw(m_background_sprite);
	window.draw(m_gui_container);
}

bool SettingsState::Update(sf::Time dt)
{
	return true;
}

bool SettingsState::HandleEvent(const sf::Event& event)
{
	bool is_key_binding = false;

	//Iterate through all of the key binding buttons to see if they are being presssed, waiting for the user to enter a key
	for (std::size_t action = 0; action < static_cast<int>(Action::kActionCount); ++action)
	{
		if (m_binding_buttons[action]->IsActive())
		{
			is_key_binding = true;
			if (event.type == sf::Event::KeyReleased)
			{
				GetContext().players_controller->AssignKey(static_cast<Action>(action), event.key.code);
				m_binding_buttons[action]->Deactivate();
			}
			break;
		}
	}

	//If pressed button changed key bindings, then update the labels
	if (is_key_binding)
	{
		UpdateLabels();
	}
	else
	{
		m_gui_container.HandleEvent(event);
	}
	return false;
}

void SettingsState::UpdateLabels()
{
	PlayersController& player = *GetContext().players_controller;
	for (std::size_t i = 0; i < static_cast<int>(Action::kActionCount); ++i)
	{
		sf::Keyboard::Key key = player.GetAssignedKey(static_cast<Action>(i));
		m_binding_labels[i]->SetText(Utility::toString(key));
	}
}

void SettingsState::AddButtonLabel(Action action, float y, const std::string& text, Context context)
{
	m_binding_buttons[static_cast<int>(action)] = std::make_shared<gui::Button>(context);
	m_binding_buttons[static_cast<int>(action)]->setPosition(390.f, y);
	m_binding_buttons[static_cast<int>(action)]->SetText(text);
	m_binding_buttons[static_cast<int>(action)]->SetToggle(true);

	m_binding_labels[static_cast<int>(action)] = std::make_shared<gui::Label>("", *context.fonts);
	m_binding_labels[static_cast<int>(action)]->setPosition(620.f, y + 15.f);

	m_gui_container.Pack(m_binding_buttons[static_cast<int>(action)]);
	m_gui_container.Pack(m_binding_labels[static_cast<int>(action)]);
}
