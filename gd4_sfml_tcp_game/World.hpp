//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceIdentifiers.hpp"
#include "ResourceHolder.hpp"
#include "SceneNode.hpp"
#include "SceneLayers.hpp"
#include "Character.hpp"
#include "TextureID.hpp"
#include "SpriteNode.hpp"
#include "CommandQueue.hpp"
#include "BloomEffect.hpp"
#include "SoundPlayer.hpp"

#include <array>
#include "PlayersController.hpp"

class World : private sf::NonCopyable
{
public:
	explicit World(sf::RenderTarget& target, FontHolder& font, SoundPlayer& sounds);
	void Update(sf::Time dt);
	void Draw();

	CommandQueue& GetCommandQueue();

	bool HasAlivePlayerOne() const;
	bool HasAlivePlayerTwo() const;

	GameRecords GetGameRecords() const;
private:
	void LoadTextures();
	void BuildScene();
	void AdaptPlayerPosition();
	void AdaptPlayerVelocity();

	void InitializeLayers();
	void BuildSnowLandscape();
	void BuildTreesRandom(sf::FloatRect lake_bounds);
	void BuildTreesFixed(sf::FloatRect lake_bounds);
	void 
		ickup(SceneNode& node, const TextureHolder& textures) const;
	sf::FloatRect GetViewBounds() const;
	sf::FloatRect GetBattleFieldBounds() const;

	void DestroyEntitiesOutsideView();

	void HandleCollisions();
	void UpdateSounds();

	void CheckPickupDrop(sf::Time dt);


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

private:
	sf::RenderTarget& m_target;
	sf::RenderTexture m_scene_texture;
	sf::View m_camera;
	TextureHolder m_textures;
	FontHolder& m_fonts;
	SoundPlayer& m_sounds;
	SceneNode m_scenegraph;
	std::array<SceneNode*, static_cast<int>(SceneLayers::kLayerCount)> m_scene_layers;
	sf::FloatRect m_world_bounds;
	sf::Vector2f m_centre_position;


	CommandQueue m_command_queue;

	CommandQueue m_create_pickup_command;
	sf::Time m_pickup_drop_interval;
	sf::Time m_time_since_last_drop;
	int m_pickups_spawned;
	int m_max_pickups;

	//std::vector<SpawnPoint> m_enemy_spawn_points;
	//std::vector<Character*> m_active_enemies;

	BloomEffect m_bloom_effect;

	bool m_is_server;

	Character* m_character_one;
	
};

