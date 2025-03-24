//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"
#include "ProjectileType.hpp"
#include "Animation.hpp"
#include "ParticleNode.hpp"
#include "EmitterNode.hpp"

class Projectile : public Entity
{
public:
	Projectile(ProjectileType type, const TextureHolder& textures, sf::Int16 identifier, bool is_server);
	Projectile(ProjectileType type, const TextureHolder& textures, sf::Int16 identifier, bool is_server, ParticleNode* particle_system);

	unsigned int GetCategory() const override;
	sf::FloatRect GetBoundingRect() const override;
	float GetMaxSpeed() const;
	float GetDamage() const;
	bool IsMarkedForRemoval() const override;

	void UpdateVisuals(sf::Time dt);

	sf::Int16 GetCharacterIdentifier() const { return m_character_identifier; };
	
	

private:
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	ProjectileType m_type;
	sf::Sprite m_sprite;
	sf::Int16 m_character_identifier;
	sf::Vector2f m_target_direction;
	bool m_is_on_server;

	//GracieChaudhary
	Animation m_impact_animation;
	EmitterNode* m_emitter;
};

