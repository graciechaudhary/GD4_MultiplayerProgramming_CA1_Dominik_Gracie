#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceIdentifiers.hpp"
#include "ResourceHolder.hpp"
#include "SceneNode.hpp"
#include "SoundPlayer.hpp"
#include "SceneLayers.hpp"
#include "BloomEffect.hpp"
#include "Character.hpp"
#include "Pickup.hpp"

class WorldClient : private sf::NonCopyable
{

private:
	struct SpawnPoint
	{
		SpawnPoint(CharacterType type, float x, float y) :m_type(type), m_x(x), m_y(y)
		{

		}
		CharacterType m_type;
		float m_x;
		float m_y;
	};

	struct PickupSpawnPoint
	{
		PickupSpawnPoint(PickupType type, float x, float y):m_type(type), m_x(x), m_y(y)
		{

		}
		PickupType m_type;
		float m_x;
		float m_y;
	};

public:
	explicit WorldClient(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds);	
	void Draw();
	void Update(sf::Time dt);

	void AddCharacter(sf::Int16 identifier);
	Character* GetCharacter(sf::Int16 identifier);
	Projectile* GetProjectile(sf::Int16 identifier);
	Pickup* GetPickup(sf::Int16 identifier);
	void RemoveCharacter(sf::Int16 character_id);
	void RemoveSnowball(sf::Int16 snowball_id);
	void RemovePickup(sf::Int16 pickup_id);

	void CreateSnowball(sf::Int16 character_identifier, sf::Int16 snowball_identifier);

	void SpawnPickup(sf::Int16 pickup_identifier, PickupType type, float x, float y);

private:
	void LoadTextures();
	void BuildScene();
	void InitializeLayers();
	void BuildSnowLandscape();
	void BuildTreesRandom(sf::FloatRect lake_bounds);
	void BuildTreesFixed(sf::FloatRect lake_bounds);
	void CreatePickup(SceneNode& node, PickupSpawnPoint& spawnpoint, const TextureHolder& textures) const;
	//sf::FloatRect GetViewBounds() const;
	sf::FloatRect GetBattleFieldBounds() const;

	void UpdateSounds();

	void AdaptPlayerPosition();

private:
	sf::RenderTarget& m_target;
	sf::RenderTexture m_scene_texture;
	//sf::View m_camera;
	TextureHolder m_textures;
	FontHolder& m_fonts;
	SoundPlayer& m_sounds;
	SceneNode m_scenegraph;
	std::array<SceneNode*, static_cast<int>(SceneLayers::kLayerCount)> m_scene_layers;
	sf::FloatRect m_world_bounds;
	sf::Vector2f m_centre_position;

	BloomEffect m_bloom_effect;

	Command m_create_pickup_command;
	std::map<sf::Int16, Character*> m_characters;
	std::map<sf::Int16, Projectile*> m_projectiles;
	std::map<sf::Int16, Pickup*> m_pickups;
	std::map<sf::Int16, ParticleNode*> m_particle_systems;
	ParticleNode* m_snow_particle;
	Projectile* m_projectile_test;
	//Character* m_character;

};