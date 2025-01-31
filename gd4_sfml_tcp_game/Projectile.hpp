//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"
#include "ProjectileType.hpp"
#include "Animation.hpp"

class Projectile : public Entity
{
public:
	Projectile(ProjectileType type, const TextureHolder& textures, bool is_player_one);

	unsigned int GetCategory() const override;
	sf::FloatRect GetBoundingRect() const override;
	float GetMaxSpeed() const;
	float GetDamage() const;
	bool IsMarkedForRemoval() const override;
	
	

private:
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	ProjectileType m_type;
	sf::Sprite m_sprite;
	bool m_is_player_one;
	sf::Vector2f m_target_direction;
	Animation m_impact_animation;
	
};

