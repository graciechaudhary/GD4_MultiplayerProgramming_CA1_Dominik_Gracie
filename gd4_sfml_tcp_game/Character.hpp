//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "Entity.hpp"
#include "CharacterType.hpp"
#include "ResourceIdentifiers.hpp"
#include "TextNode.hpp"
#include "Utility.hpp"
#include "ProjectileType.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include "Animation.hpp"
#include "CharacterAnimation.hpp"
#include "FacingDirections.hpp"
#include "ResourceNode.hpp"

class Character : public Entity
{
public:
	Character(CharacterType type, const TextureHolder& textures, const FontHolder& fonts, bool is_player_one);
	unsigned int GetCategory() const override;

	int GetMaxHitpoints() const;
	float GetMaxSpeed() const;

	void Throw();

	void RechargeSnowballs();

	void CreateSnowball(SceneNode& node, const TextureHolder& textures) const;

	sf::FloatRect GetBoundingRect() const override;
	bool IsMarkedForRemoval() const override;
	void PlayLocalSound(CommandQueue& commands, SoundEffect effect);

	void ClearWalkingFlags(sf::Time dt);

	int GetWalkingFlagsCount() const;

	void HandleSliding();

	void HandleBorderInteraction(sf::FloatRect view_bounds);

	FacingDirections GetFacingDirection() const;

	void WalkLeft();
	void WalkRight();
	void WalkUp();
	void WalkDown();

	void SetColour(sf::Color colour);
	
	void Impacted();

	sf::Color GetColour();


private:
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	void CheckProjectileLaunch(sf::Time dt, CommandQueue& commands);

	void UpdateAnimation(sf::Time dt);
	void UpdateWalkingAnimation(sf::Time dt);
	void UpdateAttackingAnimation(sf::Time dt);
	void UpdateImpactAnimation(sf::Time dt);

	void UpdateCurrentDirection();

private:
	CharacterType m_type;
	sf::Sprite m_sprite;
	Animation m_explosion;
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
	sf::Time m_blink_timer;
	sf::Time m_impact_timer;
	sf::Time m_impact_duration;

	bool m_is_player_one;

	sf::Color m_colour;
};

