#pragma once
#include <SFML/Graphics.hpp>
#include "PlayersController.hpp"
#include "Character.hpp"
#include "Pickup.hpp"
#include "Projectile.hpp"

class WorldServer : private sf::NonCopyable
{
public:
	explicit WorldServer();
	void Update(sf::Time dt);

	CommandQueue& GetCommandQueue();
	GameRecords GetGameRecords() const;

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
	
	void AdaptPlayerPosition();
	void AdaptPlayerVelocity();

	sf::FloatRect GetViewBounds() const;
	sf::FloatRect GetBattleFieldBounds() const;

	void HandleCollisions();
	
	void DestroyEntitiesOutsideView();

	//pickup 
	void CheckPickupDrop(sf::Time dt);
	void SpawnPickup();
	//void SendPickupDataToClients(PickupSpawnPoint spawnpoint);
	


private:

	sf::FloatRect m_world_bounds;
	sf::Vector2f m_centre_position;
	CommandQueue m_command_queue;

	SceneNode m_scenegraph;

	
	sf::Time m_pickup_drop_interval;
	sf::Time m_time_since_last_drop;
	int m_pickups_spawned;
	int m_max_pickups;

	std::map<std::size_t, Character*> m_characters;


};

