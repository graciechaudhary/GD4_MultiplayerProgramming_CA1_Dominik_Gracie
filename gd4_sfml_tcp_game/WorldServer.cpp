#include "WorldServer.hpp"
#include "NetworkProtocol.hpp"


#include <iostream>
#include "DataTables.hpp"

namespace {
	std::map<int, SpawnPoint> Table = InitializeSpawnPoints();
}

WorldServer::WorldServer() : m_scenegraph()
, m_command_queue()
, m_pickup_drop_interval(sf::seconds(5.f))
, m_time_since_last_drop(sf::Time::Zero)
, m_max_pickups(2)
, m_pickups_spawned(0)
, m_scene_layers()
, m_world_bounds(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)
, m_event_queue()

{
	InitializeLayers();
	LoadTextures();
}

void WorldServer::LoadTextures()
{
	m_textures.Load(TextureID::kHealthRefill, "MediaFiles/Textures/UI/HealthPickupV2.png");
	m_textures.Load(TextureID::kSnowballRefill, "MediaFiles/Textures/UI/SnowballPickup.png");

	//m_textures.Load(TextureID::kEntities, "MediaFiles/Textures/Entities.png");
	m_textures.Load(TextureID::kExplosion, "MediaFiles/Textures/Explosion/Explosion.png");
	m_textures.Load(TextureID::kImpact, "MediaFiles/Textures/Explosion/Impact.png");

	//edited texture for the snow particle effect - GracieChaudhary
	m_textures.Load(TextureID::kParticle, "MediaFiles/Textures/Particles/SnowBits.png");

	//reloading textures for game assets - GracieChaudhary
	m_textures.Load(TextureID::kCharacterMovement, "MediaFiles/Textures/Character/CharacterMovementSheet.png");
	m_textures.Load(TextureID::kSnowball, "MediaFiles/Textures/Weapon/Snowball.png");
	m_textures.Load(TextureID::kSnowTile, "MediaFiles/Textures/Environment/SnowTile_64x64.png");
	m_textures.Load(TextureID::kLakeTile, "MediaFiles/Textures/Environment/LakeTile_64x64.png");
	m_textures.Load(TextureID::kPurpleTree, "MediaFiles/Textures/Tree/PurpleTree_64x64.png");
	m_textures.Load(TextureID::kGreenTree, "MediaFiles/Textures/Tree/GreenTree_64x64.png");
	m_textures.Load(TextureID::kDeadTree, "MediaFiles/Textures/Tree/DeadTree_64x64.png");
}

void WorldServer::InitializeLayers()
{
	for (std::size_t i = 0; i < static_cast<int>(SceneLayers::kLayerCount); ++i)
	{
		ReceiverCategories category = (i == static_cast<int>(SceneLayers::kParticles)) ? ReceiverCategories::kScene : ReceiverCategories::kNone;
		SceneNode::Ptr layer(new SceneNode(category));
		m_scene_layers[i] = layer.get();
		m_scenegraph.AttachChild(std::move(layer));
	}
}

void WorldServer::Update(sf::Time dt)
{
	DestroyEntitiesOutsideView();
	CheckPickupDrop(dt);

	//Forward commands to the scenegraph
	while (!m_command_queue.IsEmpty())
	{
		m_scenegraph.OnCommand(m_command_queue.Pop(), dt);
	}

	AdaptPlayerVelocity();

	HandleCollisions();

	//check stuff that is marked for removal and then remove it
	CheckMarkedForRemoval();
	//send message to all the clients that hit has been removed from the map
	m_scenegraph.RemoveWrecks();


	m_scenegraph.Update(dt, m_command_queue);
	AdaptPlayerPosition();

}

CommandQueue& WorldServer::GetCommandQueue()
{
    return m_command_queue;
}



GameRecords WorldServer::GetGameRecords() const
{
    GameRecords records{};
    return records;
}

void WorldServer::AdaptPlayerPosition()
{

    //m_character_one->HandleBorderInteraction(view_bounds);
	for (auto chars: m_characters)
	{
		chars.second->HandleBorderInteraction(m_world_bounds);
	}
	
}

void WorldServer::AdaptPlayerVelocity()
{
}

//sf::FloatRect WorldServer::GetViewBounds() const
//{
//    return sf::FloatRect();
//}

sf::FloatRect WorldServer::GetBattleFieldBounds() const
{
	return m_world_bounds;
}

bool MatchesCategories(SceneNode::Pair& colliders, ReceiverCategories type1, ReceiverCategories type2)
{
	unsigned int category1 = colliders.first->GetCategory();
	unsigned int category2 = colliders.second->GetCategory();

	if (static_cast<int>(type1) & category1 && static_cast<int>(type2) & category2)
	{
		return true;
	}
	else if (static_cast<int>(type1) & category2 && static_cast<int>(type2) & category1)
	{
		std::swap(colliders.first, colliders.second);
	}
	else
	{
		return false;
	}
}

void WorldServer::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_scenegraph.CheckSceneCollision(m_scenegraph, collision_pairs);
	for (SceneNode::Pair pair : collision_pairs)
	{
		//On player on player collision make them bounce of each other
		if (MatchesCategories(pair, ReceiverCategories::kPlayer, ReceiverCategories::kPlayer))
		{
			auto& player_one = static_cast<Character&>(*pair.first);
			auto& playuer_two = static_cast<Character&>(*pair.second);
			//Collision response
			sf::Vector2f velocity_one = player_one.GetVelocity();
			sf::Vector2f velocity_two = playuer_two.GetVelocity();

			//Reset velocity
			player_one.SetVelocity(0.f, 0.f);
			playuer_two.SetVelocity(0.f, 0.f);

			//If one player was stationery bounce the moving player of the stationary player
			if (velocity_one == sf::Vector2f(0.f, 0.f))
			{
				velocity_one = -velocity_two;
			}
			if (velocity_two == sf::Vector2f(0.f, 0.f))
			{
				velocity_two = -velocity_one;
			}

			//Exchange velocities to bounce/push each other
			player_one.Accelerate(velocity_two);
			playuer_two.Accelerate(velocity_one);

			player_one.PlayLocalSound(m_command_queue, SoundEffect::kExplosion2);
		}
		//Pickup and apply pickup
		else if (MatchesCategories(pair, ReceiverCategories::kPlayer, ReceiverCategories::kPickup))
		{
			auto& player = static_cast<Character&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);
			//Collision response
			pickup.Apply(player);
			SoundEffect pickupSoundEffect = pickup.GetPickupType() == PickupType::kHealthRefill ? SoundEffect::kHealthPickup : SoundEffect::kSnowballPickup;
			m_pickups_spawned--;
			pickup.Destroy();
			player.PlayLocalSound(m_command_queue, pickupSoundEffect);
		}
		//On player snowball collision do damage and push player, and destroy snowball
		else if (MatchesCategories(pair, ReceiverCategories::kPlayer, ReceiverCategories::kProjectile))
		{
			auto& character = static_cast<Character&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);

			if (character.GetIdentifier() == projectile.GetCharacterIdentifier()) return;

			//Collision response
			character.Damage(projectile.GetDamage());
			character.SetVelocity(0.f, 0.f);
			character.Accelerate(projectile.GetVelocity() / (3.f, 3.f));
			//character.PlayLocalSound(m_command_queue, SoundEffect::kSnowballHitPlayer);
			//character.Impacted();
			projectile.Destroy();

			Packet_Ptr packet = std::make_unique<sf::Packet>();
			*packet << static_cast<sf::Int16>(Server::PacketType::kHealthDown);
			*packet << static_cast<sf::Int16>(character.GetIdentifier());
			m_event_queue.push_back(std::move(packet));

			std::cout << "HP:: " << static_cast<sf::Int16>(character.GetHitPoints()) << std::endl;
		}

	}

}

void WorldServer::CheckPickupDrop(sf::Time dt)
{	// Check if it's time to spawn a new pickup
	if (m_time_since_last_drop > m_pickup_drop_interval)
	{
		m_time_since_last_drop = sf::Time::Zero;
		//SpawnPickup();
		m_pickups_spawned++;
		
		//m_command_queue.Push(m_create_pickup_command);
	}

	//When no pickup on the lake speed up spawn
	if (m_pickups_spawned == 0)
	{
		m_time_since_last_drop += dt;
	}

	if (m_pickups_spawned < m_max_pickups)
	{
		m_time_since_last_drop += dt;
	}
}

void WorldServer::CheckMarkedForRemoval()
{
	//removing characters that have been destoryed
	for (auto it = m_characters.begin(); it != m_characters.end(); )
	{
		if (it->second->IsMarkedForRemoval()) 
		{
			std::cout << "Removing character with ID: " << it->first << std::endl;

			Packet_Ptr packet = std::make_unique<sf::Packet>();
			*packet << static_cast<sf::Int16>(Server::PacketType::kCharacterRemoved);
			*packet << static_cast<sf::Int16>(it->first); 
			m_event_queue.push_back(std::move(packet));

			//delete it->second;  
			it = m_characters.erase(it); 
		}
		else
		{
			++it;
		}
	}

	//removing the snowballs that have been destroyed
	for (auto it = m_projectiles.begin(); it != m_projectiles.end(); )
	{
		if (it->second->IsMarkedForRemoval())
		{
			std::cout << "[DEBUG] Removing projectile with ID: " << it->first << std::endl;

			Packet_Ptr packet = std::make_unique<sf::Packet>();
			*packet << static_cast<sf::Int16>(Server::PacketType::kSnowballRemoved);
			*packet << static_cast<sf::Int16>(it->first); 
			m_event_queue.push_back(std::move(packet));

			//delete it->second;
			it = m_projectiles.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void WorldServer::AddCharacter(sf::Int16 identifier)
{
	std::unique_ptr<Character> leader(new Character(true, identifier, m_textures, &m_event_queue, &m_projectiles));
	Character* character = leader.get();
	character->setPosition(Table[identifier].m_x, Table[identifier].m_y);
	character->SetVelocity(0, 0);
	m_characters[identifier] = character;
	m_scene_layers[static_cast<int>(SceneLayers::kIntreacations)]->AttachChild(std::move(leader));

	std::cout << character->GetWorldPosition().x << "  " << character->GetWorldPosition().y << std::endl;
}

Character* WorldServer::GetCharacter(sf::Int16 identifier)
{
	return m_characters[identifier];
}

Projectile* WorldServer::GetProjectile(sf::Int16 identifier)
{
	return m_projectiles[identifier];
}

const std::map<sf::Int16, Character*>& WorldServer::GetCharacters() const
{
	return m_characters;
}
	

std::map<sf::Int16, Projectile*>& WorldServer::GetProjectiles()
{
	return m_projectiles;
}



//void WorldServer::SpawnPickup()
//{
//	float border_distance = 65.f;
//	float x = Utility::RandomInt(GetViewBounds().width - border_distance * 2) + border_distance;
//	float y = Utility::RandomInt(GetViewBounds().height - border_distance * 2) + border_distance;
//	auto type = static_cast<PickupType>(Utility::RandomInt(static_cast<int>(PickupType::kPickupCount)));
//
//	PickupSpawnPoint spawnpoint{type,x,y};
//
//}


void WorldServer::DestroyEntitiesOutsideView()
{
	Command command;
	command.category = static_cast<int>(ReceiverCategories::kProjectile);
	command.action = DerivedAction<Entity>([this](Entity& e, sf::Time dt)
		{
			//Does the object intersect with the battlefield
			if (!GetBattleFieldBounds().intersects(e.GetBoundingRect()))
			{
				e.Destroy();
			}
		});
	m_command_queue.Push(command);

	//we will have to inform the client that entity has been destroyed by sending respective packet
}
