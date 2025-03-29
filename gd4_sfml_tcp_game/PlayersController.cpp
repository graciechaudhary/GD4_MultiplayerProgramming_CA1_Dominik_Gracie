//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "PlayersController.hpp"
#include "ReceiverCategories.hpp"
#include "Character.hpp"
#include <iostream>
#include "ParticleNode.hpp"
#include "NetworkProtocol.hpp"
#include <SFML/Network/Packet.hpp>


enum class Direction
{
	kLeft,
	kRight,
	kUp,
	kDown
};

//Dominik Hampejs D00250604
struct CharacterMover
{
	CharacterMover(Direction dir, sf::Int16 identifier) : direction(dir), identifier(identifier)
    {}
    void operator()(Character& character, sf::Time) const
    {
        if (character.GetIdentifier() != identifier) return;

		//Set flags for the direction the player is moving in
        switch (direction)
        {
        case Direction::kLeft:
            character.WalkLeft();
            break;
        case Direction::kRight:
            character.WalkRight();
            break;
        case Direction::kUp:
            character.WalkUp();
            break;
        case Direction::kDown:
            character.WalkDown();
            break;
        default:
            break;
        }

		//Get the number of flags set (buttons pressed)
        int sum = character.GetWalkingFlagsCount();

        sf::Vector2f current_velocity = character.GetVelocity();
		sf::Vector2f velocity = sf::Vector2f(0.f,0.f);
		float max_speed = 200.f; // = character.GetMaxSpeed();
		float acceleration = 10.f;


		//If the player is moving diagonally, reduce the speed and acceleration
        if (sum == 2)
        {
            max_speed /= std::sqrt(2.f);
            acceleration /= std::sqrt(2.f);
        }
        else if (current_velocity.x != 0.f && current_velocity.y != 0.f)
        {
            max_speed /= std::sqrt(2.f);
        }

		//Set the velocity based on the direction the player is moving in
        switch (direction)
        {
        case Direction::kLeft: 
			if (current_velocity.x < -max_speed)
			{
				character.SetVelocity(-max_speed, current_velocity.y);
			}
			else
			{
				velocity.x -= acceleration;
			}
            break;
        case Direction::kRight:
            if (current_velocity.x > max_speed)
            {
                character.SetVelocity(max_speed, current_velocity.y);
            }
            else
            {
				velocity.x += acceleration;
            }
            break;
        case Direction::kUp:
			if (current_velocity.y < -max_speed)
			{
				character.SetVelocity(current_velocity.x, -max_speed);
			}
			else
			{
				velocity.y -= acceleration;
			}
            break;
        case Direction::kDown:
            if (current_velocity.y > max_speed)
            {
                character.SetVelocity(current_velocity.x, max_speed);
            }
            else
            {
				velocity.y += acceleration;
            }
            break;
        default:
            break;
        }

        character.Accelerate(velocity);
    }

	Direction direction;
	sf::Int16 identifier;
};

struct CharacterThrower
{
	CharacterThrower(sf::Int16 identifier) : identifier(identifier)
	{
	}
	void operator()(Character& character, sf::Time) const
	{
        if (character.GetIdentifier() != identifier) return;

		character.Throw();
	}

    sf::Int16 identifier;
};

PlayersController::PlayersController() : m_current_game_status(GameStatus::kGameRunning), m_should_update_colours(false), m_socket(nullptr), m_identifier(1)
{
    InitialiseActions();
}

PlayersController::PlayersController(sf::TcpSocket* socket, sf::Int16 identifier) : m_should_update_colours(false), m_socket(socket), m_identifier(identifier)
{
    InitialiseActions();
}

void PlayersController::SetConnection(sf::TcpSocket* socket, sf::Int16 identifier)
{
	m_socket = socket;
	m_identifier = identifier;
}

void PlayersController::HandleEvent(const sf::Event& event)
{
    //if (event.type == sf::Event::KeyPressed)
    //{
    //    auto found = m_key_binding.find(event.key.code);
    //    if (found != m_key_binding.end() && !IsRealTimeAction(found->second))
    //    {
    //        command_queue.Push(m_action_binding[found->second]);
    //    }
    //}

    if ((event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) && m_socket)
    {
        Action action = m_key_binding[event.key.code];
        bool isPressed = (event.type == sf::Event::KeyPressed);
        if (IsRealTimeAction(action) && m_action_proxy[action] != isPressed)
        {
            m_action_proxy[action] = isPressed;
            // Send realtime change over network
            sf::Packet packet;
            packet << static_cast<sf::Int16>(Client::PacketType::kPlayerRealtimeChange);
            packet << m_identifier;
            packet << static_cast<sf::Int16>(action);
            packet << isPressed;
            m_socket->send(packet);
        }
    }

	//HandleControllerInput(event);
}

void PlayersController::HandleRealTimeInput(CommandQueue& command_queue)
{
    //Check if any of the key bindings are pressed
    for (auto pair : m_key_binding)
    {
        if (sf::Keyboard::isKeyPressed(pair.first) && IsRealTimeAction(pair.second))
        {
            command_queue.Push(m_action_binding[pair.second]);
        }
    }
}

//Dominik Hampejs D00250604
//Handle controller input for both players
void PlayersController::HandleControllerInput(const sf::Event& event) {
    if (sf::Joystick::isConnected(0)) {
        Action action = Action::kActionCount;
		if (sf::Joystick::isButtonPressed(0, 0)) //If button A is pressed
        {
			action = Action::kThrow;
        }
		if (sf::Joystick::getAxisPosition(0, sf::Joystick::X) < -50) //If stick is moved to the left
        {
			action = Action::kMoveLeft;
        }
		if (sf::Joystick::getAxisPosition(0, sf::Joystick::X) > 50) //If stick is moved to the right
		{
			action = Action::kMoveRight;
		}
		if (sf::Joystick::getAxisPosition(0, sf::Joystick::Y) < -50) //If stick is moved up
        {
            action = Action::kMoveUp;
        }
		if (sf::Joystick::getAxisPosition(0, sf::Joystick::Y) > 50) //If stick is moved down
		{
            action = Action::kMoveDown;
		}

        if (IsRealTimeAction(action))
        {
            sf::Packet packet;
            packet << static_cast<sf::Int16>(Client::PacketType::kPlayerRealtimeChange);
            packet << m_identifier;
            packet << static_cast<sf::Int16>(action);
            packet << (event.type == sf::Event::KeyPressed);
            m_socket->send(packet);
        }
    }
}

 

void PlayersController::NetworkedRealTimeInputServer(CommandQueue& command_queue)
{
	for (auto pair : m_action_proxy)
	{
		if (pair.second)
		{
			Action action = static_cast<Action>(pair.first);
			command_queue.Push(m_action_binding[action]);
		}
	}
}

void PlayersController::RegisterRealTimeInputChange(Action action, bool state)
{
	m_action_proxy[action] = state;
}


void PlayersController::AssignKey(Action action, sf::Keyboard::Key key)
{
    //Remove keys that are currently bound to the action
    for (auto itr = m_key_binding.begin(); itr != m_key_binding.end();)
    {
        if (itr->second == action)
        {
            m_key_binding.erase(itr++);
        }
        else
        {
            ++itr;
        }
    }
    m_key_binding[key] = action;
}

sf::Keyboard::Key PlayersController::GetAssignedKey(Action action) const
{
    for (auto pair : m_key_binding)
    {
        if (pair.second == action)
        {
            return pair.first;
        }
    }
    return sf::Keyboard::Unknown;
}

void PlayersController::SetGameStatus(GameStatus status)
{
    m_current_game_status = status;
}

GameStatus PlayersController::GetGameStatus() const
{
    return m_current_game_status;
}

//Dominik Hampejs D00250604
void PlayersController::SetPlayersColours(RGBColourPtr colour_one)
{
	m_colour_one = std::move(colour_one);
	m_should_update_colours = true;
}



void PlayersController::InitialiseActions()
{
    m_key_binding[sf::Keyboard::A] = Action::kMoveLeft;
    m_key_binding[sf::Keyboard::D] = Action::kMoveRight;
    m_key_binding[sf::Keyboard::W] = Action::kMoveUp;
    m_key_binding[sf::Keyboard::S] = Action::kMoveDown;
    m_key_binding[sf::Keyboard::Space] = Action::kThrow;

    m_action_binding[Action::kMoveLeft].action = DerivedAction<Character>(CharacterMover(Direction::kLeft, m_identifier));
    m_action_binding[Action::kMoveRight].action = DerivedAction<Character>(CharacterMover(Direction::kRight, m_identifier));
    m_action_binding[Action::kMoveUp].action = DerivedAction<Character>(CharacterMover(Direction::kUp, m_identifier));
    m_action_binding[Action::kMoveDown].action = DerivedAction<Character>(CharacterMover(Direction::kDown, m_identifier));
    m_action_binding[Action::kThrow].action = DerivedAction<Character>(CharacterThrower(m_identifier));

    for (auto& pair : m_action_binding)
    {
        pair.second.category = static_cast<unsigned int>(ReceiverCategories::kPlayer);
    }
}

bool PlayersController::IsRealTimeAction(Action action)
{
    switch (action)
    {
    case Action::kMoveLeft:
    case Action::kMoveRight:
    case Action::kMoveDown:
    case Action::kMoveUp:
    case Action::kThrow:
        return true;
    default:
        return false;
    }
}

//Dominik Hampejs D00250604
//Update the colours of the players based on the colour pickers
void PlayersController::UpdateColours(CommandQueue& command_queue)
{
    if (!m_should_update_colours)
    {
        return;
    }
	//Player one colour command
    Command set_colour_one;
    set_colour_one.category = static_cast<int>(ReceiverCategories::kPlayer);
    set_colour_one.action = [this](SceneNode& node, sf::Time)
        {
            Character& character = static_cast<Character&>(node);
			if (character.GetIdentifier() == 1)
			{
				character.SetColour(m_colour_one->GetColour());
			}
        };

	//Particle colour for player one snowball particles command
	Command set_particle_colour_one;
	set_particle_colour_one.category = static_cast<int>(ReceiverCategories::kParticleSystem);
	set_particle_colour_one.action = [this](SceneNode& node, sf::Time)
		{
			ParticleNode& particle = static_cast<ParticleNode&>(node);
            if (particle.GetIdentifier() == 1)
            {
                particle.SetColor(m_colour_one->GetColour());
            }
		};

    command_queue.Push(set_colour_one);
	command_queue.Push(set_particle_colour_one);
	m_should_update_colours = false;
}

void PlayersController::SetName(std::string name)
{
	m_name = name.substr(0, 4);
}
