#include "PlayersController.hpp"
#include "ReceiverCategories.hpp"
#include "Character.hpp"
#include <iostream>


enum class Direction
{
	kLeft,
	kRight,
	kUp,
	kDown
};

struct CharacterMover
{
    CharacterMover(Direction dir) : direction(dir)
    {}
    void operator()(Character& aircraft, sf::Time) const
    {
        switch (direction)
        {
        case Direction::kLeft:
            aircraft.WalkLeft();
            break;
        case Direction::kRight:
            aircraft.WalkRight();
            break;
        case Direction::kUp:
            aircraft.WalkUp();
            break;
        case Direction::kDown:
            aircraft.WalkDown();
            break;
        default:
            break;
        }

        int sum = aircraft.GetWalkingFlagsCount();

        sf::Vector2f current_velocity = aircraft.GetVelocity();
		sf::Vector2f velocity = sf::Vector2f(0.f,0.f);
		float max_speed = 200.f; // = character.GetMaxSpeed();
		float acceleration = 10.f;


        if (sum == 2)
        {
            max_speed /= std::sqrt(2.f);
            acceleration /= std::sqrt(2.f);
        }
        else if (current_velocity.x != 0.f && current_velocity.y != 0.f)
        {
            max_speed /= std::sqrt(2.f);
        }

        switch (direction)
        {
        case Direction::kLeft: 
			if (current_velocity.x < -max_speed)
			{
				aircraft.SetVelocity(-max_speed, current_velocity.y);
			}
			else
			{
				velocity.x -= acceleration;
			}
            break;
        case Direction::kRight:
            if (current_velocity.x > max_speed)
            {
                aircraft.SetVelocity(max_speed, current_velocity.y);
            }
            else
            {
				velocity.x += acceleration;
            }
            break;
        case Direction::kUp:
			if (current_velocity.y < -max_speed)
			{
				aircraft.SetVelocity(current_velocity.x, -max_speed);
			}
			else
			{
				velocity.y -= acceleration;
			}
            break;
        case Direction::kDown:
            if (current_velocity.y > max_speed)
            {
                aircraft.SetVelocity(current_velocity.x, max_speed);
            }
            else
            {
				velocity.y += acceleration;
            }
            break;
        default:
            break;
        }

        aircraft.Accelerate(velocity);
    }

	Direction direction;
};

PlayersController::PlayersController() : m_current_mission_status(MissionStatus::kMissionRunning), m_should_update_colours(false)
{
    //Set initial key bindings
    m_key_binding[sf::Keyboard::A] = Action::kMoveLeft;
    m_key_binding[sf::Keyboard::D] = Action::kMoveRight;
    m_key_binding[sf::Keyboard::W] = Action::kMoveUp;
    m_key_binding[sf::Keyboard::S] = Action::kMoveDown;
    m_key_binding[sf::Keyboard::Space] = Action::kBulletFire;
	m_key_binding[sf::Keyboard::Left] = Action::kMoveLeft2;
	m_key_binding[sf::Keyboard::Right] = Action::kMoveRight2;
	m_key_binding[sf::Keyboard::Up] = Action::kMoveUp2;
	m_key_binding[sf::Keyboard::Down] = Action::kMoveDown2;
	m_key_binding[sf::Keyboard::RShift] = Action::kThrow2;
    
    //Set initial action bindings
    InitialiseActions();


	bool first_player = true;
    //Assign all categories to a player's aircraft
    for (auto& pair : m_action_binding)
    {
        if (pair.first == Action::kMoveLeft2) first_player = false;

        if (first_player)
        {
            pair.second.category = static_cast<unsigned int>(ReceiverCategories::kPlayerOne);
		}
        else
        {
            pair.second.category = static_cast<unsigned int>(ReceiverCategories::kPlayerTwo);
        }

    }
}

void PlayersController::HandleEvent(const sf::Event& event, CommandQueue& command_queue)
{
    if (event.type == sf::Event::KeyPressed)
    {
        auto found = m_key_binding.find(event.key.code);
        if (found != m_key_binding.end() && !IsRealTimeAction(found->second))
        {
            command_queue.Push(m_action_binding[found->second]);
        }
    }
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

void PlayersController::SetMissionStatus(MissionStatus status)
{
    m_current_mission_status = status;
}

MissionStatus PlayersController::GetMissionStatus() const
{
    return m_current_mission_status;
}

void PlayersController::SetPlayersColours(RGBColourPtr colour_one, RGBColourPtr colour_two)
{
	m_colour_one = std::move(colour_one);
	m_colour_two = std::move(colour_two);
	m_should_update_colours = true;
}



void PlayersController::InitialiseActions()
{
    m_action_binding[Action::kMoveLeft].action = DerivedAction<Character>(CharacterMover(Direction::kLeft));
    m_action_binding[Action::kMoveRight].action = DerivedAction<Character>(CharacterMover(Direction::kRight));
    m_action_binding[Action::kMoveUp].action = DerivedAction<Character>(CharacterMover(Direction::kUp));
    m_action_binding[Action::kMoveDown].action = DerivedAction<Character>(CharacterMover(Direction::kDown));
    m_action_binding[Action::kBulletFire].action = DerivedAction<Character>([](Character& a, sf::Time dt)
        {
            a.Throw();
        }
    );

	m_action_binding[Action::kMoveLeft2].action = DerivedAction<Character>(CharacterMover(Direction::kLeft));
	m_action_binding[Action::kMoveRight2].action = DerivedAction<Character>(CharacterMover(Direction::kRight));
	m_action_binding[Action::kMoveUp2].action = DerivedAction<Character>(CharacterMover(Direction::kUp));
	m_action_binding[Action::kMoveDown2].action = DerivedAction<Character>(CharacterMover(Direction::kDown));
	m_action_binding[Action::kThrow2].action = DerivedAction<Character>([](Character& a, sf::Time dt)
		{
			a.Throw();
		}
	);
}

bool PlayersController::IsRealTimeAction(Action action)
{
    switch (action)
    {
    case Action::kMoveLeft:
    case Action::kMoveRight:
    case Action::kMoveDown:
    case Action::kMoveUp:
    case Action::kBulletFire:
	case Action::kMoveLeft2:
	case Action::kMoveRight2:
	case Action::kMoveDown2:
	case Action::kMoveUp2:
    case Action::kThrow2:
        return true;
    default:
        return false;
    }
}

void PlayersController::UpdateColours(CommandQueue& command_queue)
{
    if (!m_should_update_colours)
    {
        return;
    }

    Command set_colour_one;
    set_colour_one.category = static_cast<int>(ReceiverCategories::kPlayerOne);
    set_colour_one.action = [this](SceneNode& node, sf::Time)
        {
            Character& character = static_cast<Character&>(node);
            character.SetColour(m_colour_one->GetColour());
        };

    Command set_colour_two;
    set_colour_two.category = static_cast<int>(ReceiverCategories::kPlayerTwo);
    set_colour_two.action = [this](SceneNode& node, sf::Time)
        {
            Character& character = static_cast<Character&>(node);
            character.SetColour(m_colour_two->GetColour());
        };

    command_queue.Push(set_colour_one);
    command_queue.Push(set_colour_two);
	m_should_update_colours = false;
}
