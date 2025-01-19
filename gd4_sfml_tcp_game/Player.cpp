#include "Player.hpp"
#include "ReceiverCategories.hpp"
#include "Aircraft.hpp"
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
    void operator()(Aircraft& aircraft, sf::Time) const
    {
        switch (direction)
        {
        case Direction::kLeft:
            aircraft.m_is_walking_left = true;
            break;
        case Direction::kRight:
            aircraft.m_is_walking_right = true;
            break;
        case Direction::kUp:
            aircraft.m_is_walking_up = true;
            break;
        case Direction::kDown:
            aircraft.m_is_walking_down = true;
            break;
        default:
            break;
        }

        int sum = aircraft.GetWalkingFlagsCount();

        sf::Vector2f current_velocity = aircraft.GetVelocity();
		sf::Vector2f velocity = sf::Vector2f(0.f,0.f);
		float max_speed = 200.f;
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

Player::Player(): m_current_mission_status(MissionStatus::kMissionRunning)
{
    //Set initial key bindings
    m_key_binding[sf::Keyboard::A] = Action::kMoveLeft;
    m_key_binding[sf::Keyboard::D] = Action::kMoveRight;
    m_key_binding[sf::Keyboard::W] = Action::kMoveUp;
    m_key_binding[sf::Keyboard::S] = Action::kMoveDown;
    m_key_binding[sf::Keyboard::M] = Action::kMissileFire;
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
            pair.second.category = static_cast<unsigned int>(ReceiverCategories::kPlayerAircraft);
		}
        else
        {
            pair.second.category = static_cast<unsigned int>(ReceiverCategories::kEnemyAircraft);
        }

    }
}

void Player::HandleEvent(const sf::Event& event, CommandQueue& command_queue)
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

void Player::HandleRealTimeInput(CommandQueue& command_queue)
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

void Player::AssignKey(Action action, sf::Keyboard::Key key)
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

sf::Keyboard::Key Player::GetAssignedKey(Action action) const
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

void Player::SetMissionStatus(MissionStatus status)
{
    m_current_mission_status = status;
}

MissionStatus Player::GetMissionStatus() const
{
    return m_current_mission_status;
}

void Player::InitialiseActions()
{
    m_action_binding[Action::kMoveLeft].action = DerivedAction<Aircraft>(CharacterMover(Direction::kLeft));
    m_action_binding[Action::kMoveRight].action = DerivedAction<Aircraft>(CharacterMover(Direction::kRight));
    m_action_binding[Action::kMoveUp].action = DerivedAction<Aircraft>(CharacterMover(Direction::kUp));
    m_action_binding[Action::kMoveDown].action = DerivedAction<Aircraft>(CharacterMover(Direction::kDown));
    m_action_binding[Action::kBulletFire].action = DerivedAction<Aircraft>([](Aircraft& a, sf::Time dt)
        {
            a.Fire();
        }
    );

    m_action_binding[Action::kMissileFire].action = DerivedAction<Aircraft>([](Aircraft& a, sf::Time dt)
        {
            a.LaunchMissile();
        }
    );

	m_action_binding[Action::kMoveLeft2].action = DerivedAction<Aircraft>(CharacterMover(Direction::kLeft));
	m_action_binding[Action::kMoveRight2].action = DerivedAction<Aircraft>(CharacterMover(Direction::kRight));
	m_action_binding[Action::kMoveUp2].action = DerivedAction<Aircraft>(CharacterMover(Direction::kUp));
	m_action_binding[Action::kMoveDown2].action = DerivedAction<Aircraft>(CharacterMover(Direction::kDown));
	m_action_binding[Action::kThrow2].action = DerivedAction<Aircraft>([](Aircraft& a, sf::Time dt)
		{
			a.Fire();
		}
	);
}

bool Player::IsRealTimeAction(Action action)
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
