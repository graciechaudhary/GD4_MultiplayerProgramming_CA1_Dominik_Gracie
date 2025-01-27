#pragma once
#include "Entity.hpp"
#include "AircraftType.hpp"
#include "ResourceIdentifiers.hpp"
#include "TextNode.hpp"
#include "Utility.hpp"
#include "ProjectileType.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include "Animation.hpp"
#include "FacingDirections.hpp"

class Character : public Entity
{
public:
	Character(AircraftType type, const TextureHolder& textures, const FontHolder& fonts);
	unsigned int GetCategory() const override;

	int GetMaxHitpoints() const;

	void UpdateTexts();
	float GetMaxSpeed() const;

	void Throw();

	void RechargeSnowballs();
	void CreateSnowball(SceneNode& node, const TextureHolder& textures) const;
	void CreateProjectile(SceneNode& node, ProjectileType type, float x_float, float y_offset, const TextureHolder& textures) const;

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


private:
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;
	void CheckProjectileLaunch(sf::Time dt, CommandQueue& commands);
	bool IsAllied() const;
	void UpdateRollAnimation();

	void UpdateCurrentDirection();

private:
	AircraftType m_type;
	sf::Sprite m_sprite;
	Animation m_explosion;

	TextNode* m_health_display;

	Command m_throw_command;

	bool m_is_throwing;
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
};

