#include "Character.hpp"
#include "TextureID.hpp"
#include "ResourceHolder.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include "DataTables.hpp"
#include "Projectile.hpp"
#include "PickupType.hpp"
#include "Pickup.hpp"
#include "SoundNode.hpp"

#include <iostream>

namespace
{
	const std::vector<AircraftData> Table = InitializeAircraftData();
}

TextureID ToTextureID(AircraftType type)
{
	switch (type)
	{
	case AircraftType::kEagle:
		return TextureID::kEagle;
		break;
	case AircraftType::kRaptor:
		return TextureID::kRaptor;
		break;
	case AircraftType::kAvenger:
		return TextureID::kAvenger;
		break;
	}
	return TextureID::kEagle;
}

Character::Character(AircraftType type, const TextureHolder& textures, const FontHolder& fonts)  
	: Entity(Table[static_cast<int>(type)].m_hitpoints)
	, m_type(type)
	, m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture), Table[static_cast<int>(type)].m_texture_rect)
	, m_explosion(textures.Get(TextureID::kExplosion))
	, m_health_display(nullptr)
	, m_is_throwing(false)
	, m_throw_countdown(sf::Time::Zero)
	, m_is_marked_for_removal(false)
	, m_show_explosion(true)
	, m_spawned_pickup(false)
	, m_played_explosion_sound(false)
	, m_is_walking_up(false)
	, m_is_walking_down(false)
	, m_is_walking_left(false)
	, m_is_walking_right(false)
	, m_current_direction(FacingDirections::kUp)
	, m_snowball_count(3)
{
	m_explosion.SetFrameSize(sf::Vector2i(256, 256));
	m_explosion.SetNumFrames(16);
	m_explosion.SetDuration(sf::seconds(1));
	Utility::CentreOrigin(m_sprite);
	Utility::CentreOrigin(m_explosion);

	m_throw_command.category = static_cast<int>(ReceiverCategories::kScene);
	m_throw_command.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			CreateSnowball(node, textures);
		};

	std::string* health = new std::string("");
	std::unique_ptr<TextNode> health_display(new TextNode(fonts, *health));
	m_health_display = health_display.get();
	AttachChild(std::move(health_display));

	UpdateTexts();
}

unsigned int Character::GetCategory() const
{
	if (IsAllied())
	{
		return static_cast<unsigned int>(ReceiverCategories::kPlayerOne);
	}
	return static_cast<unsigned int>(ReceiverCategories::kPlayerTwo);

}

int Character::GetMaxHitpoints() const
{
	return Table[static_cast<int>(m_type)].m_hitpoints;
}


void Character::UpdateTexts()
{
	m_health_display->SetString(std::to_string(GetHitPoints()) + "HP");
	m_health_display->setPosition(0.f, 50.f);
	m_health_display->setRotation(-getRotation());
}

float Character::GetMaxSpeed() const
{
	return Table[static_cast<int>(m_type)].m_speed;
}

void Character::Throw()
{
	m_is_throwing = true;
}

void Character::RechargeSnowballs()
{
	m_snowball_count = 3;
	m_is_throwing = false;
}

void Character::CreateSnowball(SceneNode& node, const TextureHolder& textures) const
{
	ProjectileType type = IsAllied() ? ProjectileType::kAlliedBullet : ProjectileType::kEnemyBullet;
	float x_offset = 0.f;
	float y_offset = 0.5f;
	sf::Vector2f velocity(0.f,0.f);

	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

	float snowball_speed = projectile->GetMaxSpeed();

	switch (GetFacingDirection())
	{
	case FacingDirections::kUp:
		velocity.y = -snowball_speed;
		y_offset = 0.5f;
		break;
	case FacingDirections::kDown:
		velocity.y = snowball_speed;
		y_offset = 0.5f;
		break;
	case FacingDirections::kLeft:
		velocity.x = -snowball_speed;
		y_offset = 0.5f;
		break;
	case FacingDirections::kRight:
		velocity.x = snowball_speed;
		y_offset = 0.5f;
		break;
	case FacingDirections::kUpLeft:
		velocity.x = -snowball_speed / std::sqrt(2);
		velocity.y = -snowball_speed / std::sqrt(2);
		y_offset = 0.5f;
		x_offset = -0.5f;
		break;
	case FacingDirections::kUpRight:
		velocity.x = snowball_speed / std::sqrt(2);
		velocity.y = -snowball_speed / std::sqrt(2);
		y_offset = 0.5f;
		x_offset = 0.5f;
		break;
	case FacingDirections::kDownLeft:
		velocity.x = -snowball_speed / std::sqrt(2);
		velocity.y = snowball_speed / std::sqrt(2);
		y_offset = 0.5f;
		x_offset = -0.5f;
		break;
	case FacingDirections::kDownRight:
		velocity.x = snowball_speed / std::sqrt(2);
		velocity.y = snowball_speed / std::sqrt(2);
		y_offset = 0.5f;
		x_offset = 0.5f;
		break;
	default:
		break;
	}

	sf::Vector2f offset(x_offset * m_sprite.getGlobalBounds().width, y_offset * m_sprite.getGlobalBounds().height);

	projectile->setPosition(GetWorldPosition() + offset);
	projectile->SetVelocity(velocity);
	node.AttachChild(std::move(projectile));
	
}

void Character::CreateProjectile(SceneNode& node, ProjectileType type, float x_offset, float y_offset, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));
	sf::Vector2f offset(x_offset * m_sprite.getGlobalBounds().width, y_offset * m_sprite.getGlobalBounds().height);
	sf::Vector2f velocity(0, projectile->GetMaxSpeed());

	float sign = IsAllied() ? -1.f : 1.f;
	projectile->setPosition(GetWorldPosition() + offset * sign);
	projectile->SetVelocity(velocity* sign);
	node.AttachChild(std::move(projectile));
}

sf::FloatRect Character::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

bool Character::IsMarkedForRemoval() const
{
	return IsDestroyed() && (m_explosion.IsFinished() || !m_show_explosion);
}

void Character::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (IsDestroyed() && m_show_explosion)
	{
		target.draw(m_explosion, states);
	}
	else
	{
		target.draw(m_sprite, states);
	}
}

void Character::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (IsDestroyed())
	{
		m_explosion.Update(dt);
		// Play explosion sound only once
		if (!m_played_explosion_sound)
		{
			SoundEffect soundEffect = (Utility::RandomInt(2) == 0) ? SoundEffect::kExplosion1 : SoundEffect::kExplosion2;
			PlayLocalSound(commands, soundEffect);

			m_played_explosion_sound = true;
		}
		return;
	}

	Entity::UpdateCurrent(dt, commands);
	UpdateTexts();
	//UpdateMovementPattern(dt);

	UpdateRollAnimation();

	//Check if bullets or misiles are fired
	CheckProjectileLaunch(dt, commands);
}

void Character::CheckProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	//if (!IsAllied())
	//{
	//	Fire();
	//}

	if (m_is_throwing && m_throw_countdown <= sf::Time::Zero && m_snowball_count > 0)
	{
		PlayLocalSound(commands, SoundEffect::kSnowballThrow);
		commands.Push(m_throw_command);
		m_throw_countdown += Table[static_cast<int>(m_type)].m_fire_interval /  1.f;
		m_is_throwing = false;
		--m_snowball_count;
	}
	else if (m_throw_countdown > sf::Time::Zero)
	{
		//Wait, can't fire
		m_throw_countdown -= dt;
		m_is_throwing = false;
	}
}

bool Character::IsAllied() const
{
	return m_type == AircraftType::kEagle;
}

void Character::UpdateRollAnimation()
{
	if (Table[static_cast<int>(m_type)].m_has_roll_animation)
	{
		sf::IntRect textureRect = Table[static_cast<int>(m_type)].m_texture_rect;

		//Roll left: Texture rect is offset once
		if (GetVelocity().x < 0.f)
		{
			textureRect.left += textureRect.width;
		}
		else if (GetVelocity().x > 0.f)
		{
			textureRect.left += 2 * textureRect.width;
		}
		m_sprite.setTextureRect(textureRect);

	}
}

void Character::UpdateCurrentDirection()
{
	int walking_flags = GetWalkingFlagsCount();

	if (walking_flags == 1)
	{
		if (m_is_walking_up)
		{
			m_current_direction = FacingDirections::kUp;
		}
		else if (m_is_walking_down)
		{
			m_current_direction = FacingDirections::kDown;
		}
		else if (m_is_walking_left)
		{
			m_current_direction = FacingDirections::kLeft;
		}
		else if (m_is_walking_right)
		{
			m_current_direction = FacingDirections::kRight;
		}
	}
	else if (walking_flags == 2)
	{
		if (m_is_walking_up && m_is_walking_left)
		{
			m_current_direction = FacingDirections::kUpLeft;
		}
		else if (m_is_walking_up && m_is_walking_right)
		{
			m_current_direction = FacingDirections::kUpRight;
		}
		else if (m_is_walking_down && m_is_walking_left)
		{
			m_current_direction = FacingDirections::kDownLeft;
		}
		else if (m_is_walking_down && m_is_walking_right)
		{
			m_current_direction = FacingDirections::kDownRight;
		}
	}
	else if (walking_flags == 3)
	{
		if (!m_is_walking_up)
		{
			m_current_direction = FacingDirections::kDown;
		}
		else if (!m_is_walking_down)
		{
			m_current_direction = FacingDirections::kUp;
		}
		else if (!m_is_walking_left)
		{
			m_current_direction = FacingDirections::kRight;
		}
		else if (!m_is_walking_right)
		{
			m_current_direction = FacingDirections::kLeft;
		}
	}
}

void Character::PlayLocalSound(CommandQueue& commands, SoundEffect effect)
{
	sf::Vector2f world_position = GetWorldPosition();

	Command command;
	command.category = static_cast<int>(ReceiverCategories::kSoundEffect);
	command.action = DerivedAction<SoundNode>(
		[effect, world_position](SoundNode& node, sf::Time)
		{
			node.PlaySound(effect, world_position);
		});

	commands.Push(command);
}

void Character::ClearWalkingFlags(sf::Time dt)
{
	if (m_clear_flags_time > sf::milliseconds(250))
	{
		m_is_walking_down = false;
		m_is_walking_left = false;
		m_is_walking_right = false;
		m_is_walking_up = false;
		m_clear_flags_time = sf::Time::Zero;
	}
	else
	{
		m_clear_flags_time += dt;
	}

}

int Character::GetWalkingFlagsCount() const
{
	return m_is_walking_up + m_is_walking_down + m_is_walking_left + m_is_walking_right;
}

void Character::HandleSliding()
{
	sf::Vector2f velocity = GetVelocity();
	float x_reduce = 0.f;
	float y_reduce = 0.f;
	float reduction_rate = 1.4f;

	if (velocity.x != 0.f && !m_is_walking_left && !m_is_walking_right)
	{
		x_reduce = velocity.x > 0.f ? -reduction_rate : reduction_rate;
	}
	if (velocity.y != 0.f && !m_is_walking_down && !m_is_walking_up)
	{
		y_reduce = velocity.y > 0.f ? -reduction_rate : reduction_rate;
	}

	Accelerate(x_reduce, y_reduce);


	if (GetVelocity().x <= 4.f && GetVelocity().x >= -4.f)
	{
		SetVelocity(0.f, GetVelocity().y);
	}

	if (GetVelocity().y <= 4.f && GetVelocity().y >= -4.f)
	{
		SetVelocity(GetVelocity().x, 0.f);
	}

}

void Character::HandleBorderInteraction(sf::FloatRect view_bounds)
{
	const float border_distance = 40.f;

	sf::Vector2f position = getPosition();
	position.x = std::max(position.x, view_bounds.left + border_distance);
	position.x = std::min(position.x, view_bounds.left + view_bounds.width - border_distance);
	position.y = std::max(position.y, view_bounds.top + border_distance);
	position.y = std::min(position.y, view_bounds.top + view_bounds.height - border_distance);

	if (position != getPosition())
	{
		SetVelocity(0.f, 0.f);
	}

	setPosition(position);
}

FacingDirections Character::GetFacingDirection() const
{
	return m_current_direction;
}

void Character::WalkUp()
{
	m_is_walking_up = true;
	UpdateCurrentDirection();
}

void Character::WalkDown()
{
	m_is_walking_down = true;
	UpdateCurrentDirection();
}

void Character::SetColour(sf::Color colour)
{
	m_sprite.setColor(colour);
}

void Character::WalkRight()
{
	m_is_walking_right = true;
	UpdateCurrentDirection();
}

void Character::WalkLeft()
{
	m_is_walking_left = true;
	UpdateCurrentDirection();
}