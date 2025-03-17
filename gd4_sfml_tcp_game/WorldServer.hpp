#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceIdentifiers.hpp"
#include "ResourceHolder.hpp"
#include "PlayersController.hpp"
#include "Character.hpp"
#include "Pickup.hpp"
#include "Projectile.hpp"
#include "SceneLayers.hpp"
#include "SFML/Network.hpp"
#include <array>

class WorldServer : private sf::NonCopyable
{
public:
	explicit WorldServer();
	void Update(sf::Time dt);

	CommandQueue& GetCommandQueue();
	GameRecords GetGameRecords() const;

	void AddCharacter(sf::Int16 identifier);
	Character* GetCharacter(sf::Int16 identifier);

	typedef std::unique_ptr<sf::Packet> Packet_Ptr;

	std::deque<Packet_Ptr>& GetEventQueue() { return m_event_queue; };

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
		PickupSpawnPoint(PickupType type, float x, float y) :m_type(type), m_x(x), m_y(y)
		{

		}
		PickupType m_type;
		float m_x;
		float m_y;
	};

private:

	void LoadTextures();

	void InitializeLayers();
	void AdaptPlayerPosition();
	void AdaptPlayerVelocity();

	sf::FloatRect GetViewBounds() const;
	sf::FloatRect GetBattleFieldBounds() const;

	void HandleCollisions();
	
	void DestroyEntitiesOutsideView();

	//pickup 
	void CheckPickupDrop(sf::Time dt);
	//void SpawnPickup();
	//void SendPickupDataToClients(PickupSpawnPoint spawnpoint);

	
	


private:

	sf::FloatRect m_world_bounds;
	sf::Vector2f m_centre_position;
	CommandQueue m_command_queue;

	SceneNode m_scenegraph;
	std::array<SceneNode*, static_cast<int>(SceneLayers::kLayerCount)> m_scene_layers;
	
	TextureHolder m_textures;

	sf::Time m_pickup_drop_interval;
	sf::Time m_time_since_last_drop;
	int m_pickups_spawned;
	int m_max_pickups;

	std::map<sf::Int16, Character*> m_characters;
	std::map<sf::Int16, Projectile*> m_projectiles;
	std::deque<Packet_Ptr> m_event_queue;
};

