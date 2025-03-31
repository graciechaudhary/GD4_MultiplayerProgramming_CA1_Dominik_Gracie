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

	void AddCharacter(sf::Int16 identifier,sf::Int16 place);
	Character* GetCharacter(sf::Int16 identifier);
	Projectile* GetProjectile(sf::Int16 identifier);


	const std::map<sf::Int16, Character*>& GetCharacters() const;
	std::map<sf::Int16, Projectile*>& GetProjectiles();
	
	typedef std::unique_ptr<sf::Packet> Packet_Ptr;

	std::deque<Packet_Ptr>& GetEventQueue() { return m_event_queue; };

	sf::Int16 CheckAlivePlayers();

	void RemoveCharacter(sf::Int16 character_id);

	void StartClock() { m_clock.restart(); };

	struct PlayerRecords {
		PlayerRecords() :m_survival_time(sf::Time::Zero), m_kills(0)
		{
		}
		sf::Time m_survival_time = sf::Time::Zero;
		sf::Int16 m_kills = 0;
	};

	void PrintRecords();

	void MarkWinnersScore();

	std::map<sf::Int16, PlayerRecords>& GetPlayerRecords() { return m_players_records; };


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

	void CheckMarkedForRemoval();
	void SpawnPickup();

	sf::Vector2f GetRandomPosition(float min_x, float max_x, float min_y, float max_y);




private:

	sf::FloatRect m_world_bounds;
	sf::Vector2f m_centre_position;
	CommandQueue m_command_queue;
	Command m_create_pickup_command;

	SceneNode m_scenegraph;
	std::array<SceneNode*, static_cast<int>(SceneLayers::kLayerCount)> m_scene_layers;
	
	TextureHolder m_textures;

	sf::Time m_pickup_drop_interval;
	sf::Time m_full_drop_interval;
	sf::Time m_time_since_last_drop;
	int m_pickups_spawned;
	int m_max_pickups;
	sf::Int16 m_pickup_counter;


	sf::Clock m_clock;
	std::map<sf::Int16, PlayerRecords> m_players_records;
	std::deque<sf::Int16> m_order_of_death;
	
	std::map<sf::Int16, Character*> m_characters;
	std::map<sf::Int16, Projectile*> m_projectiles;
	std::deque<Packet_Ptr> m_event_queue;

	sf::Int16 m_players_alive;
	int m_last_quadrant;
};

