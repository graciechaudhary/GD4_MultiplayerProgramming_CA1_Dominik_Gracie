//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "Entity.hpp"
#include "CharacterType.hpp"
#include "ResourceIdentifiers.hpp"
#include "TextNode.hpp"
#include "Utility.hpp"
#include "ProjectileType.hpp"
#include "Projectile.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include "Animation.hpp"
#include "CharacterAnimation.hpp"
#include "FacingDirections.hpp"
#include "ResourceNode.hpp"
#include <SFML/Network/Packet.hpp>

class Character : public Entity
{
public:
	Character(bool is_on_server, int identifier, const TextureHolder& textures, const FontHolder& fonts);
	Character(bool is_on_server, int identifier, const TextureHolder& textures, std::deque<std::unique_ptr<sf::Packet>>* event_queue);
	unsigned int GetCategory() const override;

	typedef std::shared_ptr<Character> Shared;

	int GetMaxHitpoints() const;
	float GetMaxSpeed() const;

	void Throw();

	void RechargeSnowballs();

	void CreateSnowball(SceneNode& node, std::unique_ptr<Projectile> projectile) const;

	void CreateSnowball(SceneNode& node, const TextureHolder& textures) const;

	void Damage(int damage) override;

	sf::FloatRect GetBoundingRect() const override;
	bool IsMarkedForRemoval() const override;
	void PlayLocalSound(CommandQueue& commands, SoundEffect effect);

	void ClearWalkingFlags(sf::Time dt);

	int GetWalkingFlagsCount() const;

	void HandleSliding();

	void HandleBorderInteraction(sf::FloatRect view_bounds);

	FacingDirections GetFacingDirection() const;

	int GetThrowCount();
	int GetGotHitCount();

	void WalkLeft();
	void WalkRight();
	void WalkUp();
	void WalkDown();

	void SetColour(sf::Color colour);
	
	void Impacted();

	sf::Color GetColour();

	sf::Int16 GetIdentifier() const;

	void UpdateVisuals(sf::Time dt);

	void SetCurrentDirection(FacingDirections dir) { m_current_direction = dir; };


private:
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	void CheckProjectileLaunch(sf::Time dt, CommandQueue& commands);

	void UpdateAnimation(sf::Time dt);

	//GracieChaudhary - methods to implement character animations
	void UpdateWalkingAnimation(sf::Time dt);
	void UpdateAttackingAnimation(sf::Time dt);
	void UpdateImpactAnimation(sf::Time dt);

	void UpdateCurrentDirection();

private:
	CharacterType m_type;
	sf::Sprite m_sprite;
	Animation m_explosion;

	//GracieChaudhary - new variables for character animation
	CharacterAnimationType m_current_animation;
	CharacterAnimation m_walking;
	CharacterAnimation m_attacking;

	ResourceNode* m_health_display;
	ResourceNode* m_snowball_display;


	Command m_throw_command;

	bool m_is_throwing;
	bool m_is_impacted;
	sf::Time m_throw_countdown;
	int m_snowball_count;

	bool m_is_marked_for_removal;
	bool m_show_explosion;
	bool m_spawned_pickup;
	bool m_played_explosion_sound;

	bool m_is_walking_left;
	bool m_is_walking_right;
	bool m_is_walking_up;
	bool m_is_walking_down;

	FacingDirections m_current_direction;
		
	sf::Time m_clear_flags_time;

	//GracieChaudhary - new variables for impact animation
	sf::Time m_blink_timer;
	sf::Time m_impact_timer;
	sf::Time m_impact_duration;

	sf::Color m_colour;

	int m_got_hit_count;
	int m_throw_count;

	sf::Int16 m_identifier;
	bool m_is_on_server;

	std::deque<std::unique_ptr<sf::Packet>>* m_event_queue;
};

