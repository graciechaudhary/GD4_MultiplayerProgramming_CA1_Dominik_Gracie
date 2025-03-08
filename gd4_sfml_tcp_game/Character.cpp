//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
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
	const std::vector<CharacterData> Table = InitializeCharacterData();	
}

Character::Character(bool is_on_server, int identifier, const TextureHolder& textures, const FontHolder& fonts)
	: Entity(Table[static_cast<int>(CharacterType::kDefault)].m_hitpoints)
	, m_type(CharacterType::kDefault)
	, m_sprite(textures.Get(Table[static_cast<int>(CharacterType::kDefault)].m_texture), Table[static_cast<int>(CharacterType::kDefault)].m_texture_rect)
	, m_explosion(textures.Get(TextureID::kExplosion))
	, m_current_animation(CharacterAnimationType::kWalk)
	, m_walking(CharacterAnimationType::kWalk, textures.Get(TextureID::kCharacterMovement))
	, m_attacking(CharacterAnimationType::kAttack, textures.Get(TextureID::kCharacterMovement))
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
	, m_current_direction(FacingDirections::kDown)
	, m_snowball_count(Table[static_cast<int>(CharacterType::kDefault)].max_snowballs)
	, m_impact_duration(sf::seconds(0.5f))
	, m_blink_timer(sf::Time::Zero)
	, m_identifier(identifier)
	, m_is_on_server(is_on_server)
	, m_is_impacted(false)
	{
	m_got_hit_count = 0;
	m_throw_count = 0;

	//GracieChaudhary - Walking Animation Setup
	m_walking.SetFrameSize(sf::Vector2i(38, 42));
	m_walking.SetNumFrames(4);
	m_walking.SetDuration(sf::seconds(1.f));
	m_walking.SetRepeating(true);

	//GracieChaudhary - Attacking Animation Setup
	m_attacking.SetFrameSize(sf::Vector2i(38, 42));
	m_attacking.SetNumFrames(4);
	m_attacking.SetDuration(sf::seconds(0.75f));
	m_attacking.SetRepeating(false);

	m_explosion.SetFrameSize(sf::Vector2i(100, 100));
	m_explosion.SetNumFrames(81);
	m_explosion.SetDuration(sf::seconds(1.5f));
	m_explosion.scale(2, 2);
	Utility::CentreOrigin(m_sprite);
	Utility::CentreOrigin(m_explosion);

	m_throw_command.category = static_cast<int>(ReceiverCategories::kScene);
	m_throw_command.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			CreateSnowball(node, textures);
		};

	std::unique_ptr<ResourceNode> health_display(new ResourceNode(textures, TextureID::kHealthRefill ,GetHitPoints(), 14.f, 0.3f));
	m_health_display = health_display.get();
	m_health_display->setPosition(-20.f, 25.f);
	AttachChild(std::move(health_display));

	std::unique_ptr<ResourceNode> snowball_display(new ResourceNode(textures, TextureID::kSnowball, m_snowball_count, 12.f, 0.4f));
	m_snowball_display = snowball_display.get();
	m_snowball_display->setPosition(-28.f, -34.f);
	AttachChild(std::move(snowball_display));

}

unsigned int Character::GetCategory() const
{
	return static_cast<unsigned int>(ReceiverCategories::kPlayer);

}

int Character::GetMaxHitpoints() const
{
	return Table[static_cast<int>(m_type)].m_hitpoints;
}



float Character::GetMaxSpeed() const
{
	return Table[static_cast<int>(m_type)].m_speed;
}

void Character::Throw()
{
	m_is_throwing = true;	
}

//Dominik Hampejs D00250604
void Character::RechargeSnowballs()
{
	m_snowball_count = Table[static_cast<int>(m_type)].max_snowballs;
	m_is_throwing = false;
}

void Character::CreateSnowball(SceneNode& node, const TextureHolder& textures) const
{
	ProjectileType type = ProjectileType::kSnowball;
	float x_offset = 0.f;
	float y_offset = 0.5f;
	sf::Vector2f velocity(0.f,0.f);

	std::unique_ptr<Projectile> projectile(new Projectile(type, textures, m_identifier));

	float snowball_speed = projectile->GetMaxSpeed();

	//Set the projectile velocity and offset based on the direction the player faces 
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

//Dominik Hampejs D00250604
//Override to include got hit count
void Character::Damage(int damage)
{
	Entity::Damage(damage);
	m_got_hit_count++;
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
			SoundEffect explosionEffect = (Utility::RandomInt(2) == 0) ? SoundEffect::kExplosion1 : SoundEffect::kExplosion2;
			PlayLocalSound(commands, explosionEffect);

			m_played_explosion_sound = true;
		}
		return;
	}

	Entity::UpdateCurrent(dt, commands);

	//Update resource indicators
	m_health_display->SetResource(GetHitPoints());
	m_snowball_display->SetResource(m_snowball_count);

	
	//Check if bullets or misiles are fired
	CheckProjectileLaunch(dt, commands);
	
	ClearWalkingFlags(dt);
	HandleSliding();

	UpdateAnimation(dt);

}

void Character::CheckProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	
	if (m_is_throwing && m_throw_countdown <= sf::Time::Zero && m_snowball_count > 0)
	{
		m_throw_count++;
		m_current_animation = CharacterAnimationType::kAttack; 
		PlayLocalSound(commands, SoundEffect::kSnowballThrow);
		commands.Push(m_throw_command);
		m_throw_countdown += Table[static_cast<int>(m_type)].m_fire_interval /  1.f;		
		--m_snowball_count;
		m_is_throwing = false;
	}
	else if (m_throw_countdown > sf::Time::Zero)
	{
		//Wait, can't fire
		m_throw_countdown -= dt;
		m_is_throwing = false;
		
	}

	
}


//GracieChaudhary - Implementation of different animations depending on animation type
void Character::UpdateAnimation(sf::Time dt) {

	if (m_current_animation == CharacterAnimationType::kAttack) {
		
		UpdateAttackingAnimation(dt);
	}
	else if(m_current_animation == CharacterAnimationType::kImpact){
		UpdateImpactAnimation(dt);
	}
	else
	{
		UpdateWalkingAnimation(dt);
	}
}

//GracieChaudhary - Walking animation being updated depending what way the character is facing
void Character::UpdateWalkingAnimation(sf::Time dt)
{
	if (GetVelocity() == sf::Vector2f(0.f, 0.f))
	{
		m_walking.Restart();  
	}
	else
	{
		m_walking.Update(dt);
	}

	//sets respective row of direction facing
	switch (m_current_direction)
	{
		case FacingDirections::kDown: 		m_walking.SetRow(0);		break;
		case FacingDirections::kDownLeft:		m_walking.SetRow(1);		break;
		case FacingDirections::kDownRight:		m_walking.SetRow(2);		break;
		case FacingDirections::kLeft:		m_walking.SetRow(3);		break;
		case FacingDirections::kRight:		m_walking.SetRow(4);		break;
		case FacingDirections::kUp:		m_walking.SetRow(5);		break;
		case FacingDirections::kUpLeft:		m_walking.SetRow(6);		break;
		case FacingDirections::kUpRight:		m_walking.SetRow(7);		break;
	}
	m_sprite.setTextureRect(m_walking.GetCurrentTextureRect());
}

//GracieChaudhary - gets called when character is attacking, varies with character direction
void Character::UpdateAttackingAnimation(sf::Time dt)
{	
	m_attacking.Update(dt);	

	//checks direction and sets appropriate row
	switch (m_current_direction)
	{
	case FacingDirections::kDown:      m_attacking.SetRow(8); break;
	case FacingDirections::kDownLeft:  m_attacking.SetRow(10); break;
	case FacingDirections::kDownRight: m_attacking.SetRow(8); break;
	case FacingDirections::kLeft:      m_attacking.SetRow(10); break;
	case FacingDirections::kRight:     m_attacking.SetRow(9); break;
	case FacingDirections::kUp:        m_attacking.SetRow(11); break;
	case FacingDirections::kUpLeft:    m_attacking.SetRow(11); break;
	case FacingDirections::kUpRight:   m_attacking.SetRow(9); break;
	}

	m_sprite.setTextureRect(m_attacking.GetCurrentTextureRect());
	
	//sets back walking animation
	if (m_attacking.GetCurrentFrame() == 3) {
		m_current_animation = CharacterAnimationType::kWalk;
		m_attacking.Restart();
	}
}

//GracieChaudhary - this method affects the UpdateWalkingAnimation with a blinking effect to display that the character has been hit by a snowball
void Character::UpdateImpactAnimation(sf::Time dt)
{
	//Sets back walking animation
	if (m_current_animation != CharacterAnimationType::kImpact)
	{
		m_walking.Update(dt); 
	}

	m_blink_timer += dt;
	m_impact_timer += dt;

	//hanges between original colour and transparency
	if (m_blink_timer >= sf::seconds(0.1f))
	{
		m_sprite.setColor(m_sprite.getColor() == sf::Color::Transparent ? m_colour : sf::Color::Transparent);
		m_blink_timer = sf::Time::Zero;
	}

	//Sets back after impact duration
	if (m_impact_timer >= m_impact_duration)
	{
		m_current_animation = CharacterAnimationType::kWalk; 
		m_sprite.setColor(m_colour);
		m_impact_timer = sf::Time::Zero;  
	}

	m_sprite.setTextureRect(m_walking.GetCurrentTextureRect());
	
}



//Dominik Hampejs D00250604
//Set the currention direction that the character is facing
void Character::UpdateCurrentDirection()
{
	//Get buttons pressed
	int walking_flags = GetWalkingFlagsCount();

	//One button pressed = the 4 cardinal directions
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
	//Two buttons pressed = diagonal movement
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
	//Three button pressed = opposite buttons are ignored and the third decides direction
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

//Dominik Hampejs D00250604
//SFML has trouble detecting what button are truly pressed this fixes the issue of button presenting as being pressed when it is not
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

//Dominik Hampejs D00250604
//Return all the current walking direction (buttons pressed)
int Character::GetWalkingFlagsCount() const
{
	return m_is_walking_up + m_is_walking_down + m_is_walking_left + m_is_walking_right;
}

//Dominik Hampejs D00250604
//Decelerate the character
void Character::HandleSliding()
{
	sf::Vector2f velocity = GetVelocity();
	float x_reduce = 0.f;
	float y_reduce = 0.f;
	float reduction_rate = 1.4f;

	//If the player is not walking on the x axis reduce velocity towards 0
	if (velocity.x != 0.f && !m_is_walking_left && !m_is_walking_right)
	{
		x_reduce = velocity.x > 0.f ? -reduction_rate : reduction_rate;
	}
	//If the player is not walking on the y axis reduce velocity towards 0
	if (velocity.y != 0.f && !m_is_walking_down && !m_is_walking_up)
	{
		y_reduce = velocity.y > 0.f ? -reduction_rate : reduction_rate;
	}

	Accelerate(x_reduce, y_reduce);


	//If velocity is low enough just stop the character
	if (GetVelocity().x <= 4.f && GetVelocity().x >= -4.f)
	{
		SetVelocity(0.f, GetVelocity().y);
	}

	if (GetVelocity().y <= 4.f && GetVelocity().y >= -4.f)
	{
		SetVelocity(GetVelocity().x, 0.f);
	}

}

//Dominik Hampejs D00250604
//Stop the player when the hit the border of the lake
void Character::HandleBorderInteraction(sf::FloatRect view_bounds)
{
	const float border_distance = 70.f;

	sf::Vector2f position = getPosition();
	position.x = std::max(position.x, view_bounds.left + border_distance);
	position.x = std::min(position.x, view_bounds.left + view_bounds.width - border_distance);
	position.y = std::max(position.y, view_bounds.top + border_distance - 10);
	position.y = std::min(position.y, view_bounds.top + view_bounds.height - border_distance - 5);

	if (position != getPosition())
	{
		SetVelocity(0.f, 0.f);
	}

	setPosition(position);
}

//Dominik Hampejs D00250604
FacingDirections Character::GetFacingDirection() const
{
	return m_current_direction;
}

//Dominik Hampejs D00250604
int Character::GetThrowCount()
{
	return m_throw_count;
}

//Dominik Hampejs D00250604
int Character::GetGotHitCount()
{
	return m_got_hit_count;
}

//Dominik Hampejs D00250604
void Character::WalkUp()
{
	m_is_walking_up = true;
	UpdateCurrentDirection();
}

//Dominik Hampejs D00250604
void Character::WalkDown()
{
	m_is_walking_down = true;
	UpdateCurrentDirection();
}

//GraacieChaudhary - sets current animation to impact
void Character::Impacted() {
	m_current_animation = CharacterAnimationType::kImpact;
}

//Dominik Hampejs D00250604
void Character::SetColour(sf::Color colour)
{
	m_sprite.setColor(colour);
	m_colour = colour;
}

//Dominik Hampejs D00250604
sf::Color Character::GetColour()
{
	return m_colour;
}

int Character::GetIdentifier() const
{
	return m_identifier;
}

//Dominik Hampejs D00250604
void Character::WalkRight()
{
	m_is_walking_right = true;
	UpdateCurrentDirection();
}

//Dominik Hampejs D00250604
void Character::WalkLeft()
{
	m_is_walking_left = true;
	UpdateCurrentDirection();
}