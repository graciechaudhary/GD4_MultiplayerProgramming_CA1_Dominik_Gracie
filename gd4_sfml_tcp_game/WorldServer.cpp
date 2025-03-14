#include "WorldServer.hpp"
#include "NetworkProtocol.hpp"


#include <iostream>

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
}

void WorldServer::LoadTextures()
{
	//edited texture for the snow particle effect - GracieChaudhary
	m_textures.Load(TextureID::kParticle, "MediaFiles/Textures/Particles/SnowBits.png");

	//reloading textures for game assets - GracieChaudhary
	m_textures.Load(TextureID::kCharacterMovement, "MediaFiles/Textures/Character/CharacterMovementSheet.png");
	m_textures.Load(TextureID::kSnowball, "MediaFiles/Textures/Weapon/Snowball.png");
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
			//Collision response
			character.Damage(projectile.GetDamage());
			character.SetVelocity(0.f, 0.f);
			character.Accelerate(projectile.GetVelocity() / (3.f, 3.f));
			character.PlayLocalSound(m_command_queue, SoundEffect::kSnowballHitPlayer);
			character.Impacted();
			projectile.Destroy();

			Packet_Ptr packet;
			*packet << static_cast<sf::Int16>(Server::PacketType::kHealthChange);
			*packet << character.GetIdentifier() << static_cast<sf::Int16>(character.GetHitPoints());
			m_event_queue.push_back(std::move(packet));


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

void WorldServer::AddCharacter(sf::Int16 identifier)
{
	std::unique_ptr<Character> leader(new Character(true, identifier));
	Character* character = leader.get();
	character->setPosition(100.f, 100.f);
	character->SetVelocity(0, 0);
	m_characters[identifier] = character;
	m_scene_layers[static_cast<int>(SceneLayers::kIntreacations)]->AttachChild(std::move(leader));

	std::cout << character->GetWorldPosition().x << "  " << character->GetWorldPosition().y << std::endl;
}

Character* WorldServer::GetCharacter(sf::Int16 identifier)
{
	return m_characters[identifier];
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
