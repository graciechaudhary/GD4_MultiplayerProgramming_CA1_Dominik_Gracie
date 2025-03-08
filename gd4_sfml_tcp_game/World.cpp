//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "World.hpp"
#include "Pickup.hpp"
#include "Projectile.hpp"
#include "ParticleNode.hpp"
#include "SoundNode.hpp"
#include <set> 
#include <iostream> 
#include "EmitterNode.hpp"
#include <cstdlib> 

World::World(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds)
	:m_target(output_target)
	,m_camera(output_target.getDefaultView())
	,m_textures()
	,m_fonts(font)
	,m_sounds(sounds)
	,m_scenegraph(ReceiverCategories::kNone)
	,m_scene_layers()
	,m_world_bounds(0.f,0.f, m_camera.getSize().x, m_camera.getSize().y)
	,m_centre_position(m_camera.getSize().x/2.f, m_camera.getSize().y/2.f)
	,m_character_one(nullptr)
	, m_time_since_last_drop(sf::Time::Zero)
	, m_pickup_drop_interval(sf::seconds(5.f))
	, m_max_pickups(3)
	, m_pickups_spawned(0)
{
	m_scene_texture.create(m_target.getSize().x, m_target.getSize().y);
	LoadTextures();
	BuildScene();
	m_camera.setCenter(m_centre_position);

	m_create_pickup_command.category = static_cast<int>(ReceiverCategories::kScene);
	m_create_pickup_command.action = [this](SceneNode& node, sf::Time)
		{
			CreatePickup(node, m_textures);
		};
}

void World::Update(sf::Time dt)
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

void World::Draw()
{
	if (PostEffect::IsSupported())
	{
		m_scene_texture.clear();
		m_scene_texture.setView(m_camera);
		m_scene_texture.draw(m_scenegraph);
		m_scene_texture.display();
		m_bloom_effect.Apply(m_scene_texture, m_target);
		//m_shadow_effect.Apply(m_scene_texture, m_target);

	}
	else
	{
		m_target.setView(m_camera);
		m_target.draw(m_scenegraph);
	}

	/*m_target.setView(m_camera);
	m_target.draw(m_scenegraph);*/
}

CommandQueue& World::GetCommandQueue()
{
	return m_command_queue;
}

//Dominik Hampejs D00250604
bool World::HasAlivePlayerOne() const
{
	return !m_character_one->IsMarkedForRemoval();
}

//Dominik Hampejs D00250604
bool World::HasAlivePlayerTwo() const
{
	return true;//!m_character_two->IsMarkedForRemoval();
}

//Dominik Hampejs D00250604
//Return statistic about the game
GameRecords World::GetGameRecords() const
{
	GameRecords records{};
	records.player_one_hit = 1;
	records.player_one_throw = 1;
	records.player_two_hit = 1;
	records.player_two_throw = 1;
	return records;
}

void World::LoadTextures()
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

void World::BuildScene()
{
	//GracieChaudhary - cleaning the code
	InitializeLayers();

	//builds snow landsacpe, adds tree sprite - Gracie Chaudhary
	BuildSnowLandscape();	

	//Add the player's aircraft
	std::unique_ptr<Character> leader(new Character(false, 1 , m_textures, m_fonts));
	m_character_one = leader.get();
	m_character_one->setPosition(m_centre_position.x - 230, m_centre_position.y);
	m_character_one->SetVelocity(0, 0);
	m_scene_layers[static_cast<int>(SceneLayers::kIntreacations)]->AttachChild(std::move(leader));
	

	/*std::unique_ptr<Character> second(new Character(false, 2, m_textures, m_fonts));
	m_character_two = second.get();
	m_character_two->setPosition(m_centre_position.x + 220, m_centre_position.y);
	m_character_two->SetVelocity(0, 0);
	m_scene_layers[static_cast<int>(SceneLayers::kIntreacations)]->AttachChild(std::move(second));*/

	
	std::unique_ptr<ParticleNode> snowNode(new ParticleNode(ParticleType::kSnow, m_textures, 1));
	m_scene_layers[static_cast<int>(SceneLayers::kParticles)]->AttachChild(std::move(snowNode));

	// Add sound effect node
	std::unique_ptr<SoundNode> soundNode(new SoundNode(m_sounds));
	m_scenegraph.AttachChild(std::move(soundNode));
		
}

//cleaning up the code - GracieChaudhary
void World::InitializeLayers() {
	//Initialize the different layers
	for (std::size_t i = 0; i < static_cast<int>(SceneLayers::kLayerCount); ++i)
	{
		ReceiverCategories category = (i == static_cast<int>(SceneLayers::kParticles)) ? ReceiverCategories::kScene : ReceiverCategories::kNone;
		SceneNode::Ptr layer(new SceneNode(category));
		m_scene_layers[i] = layer.get();
		m_scenegraph.AttachChild(std::move(layer));
	}
}

//building our snow landscape - snow, icy lake, trees - GracieChaudhary
void World::BuildSnowLandscape() {

	//Prepare the background - defining the area for the repeated texture, creating sprite using the repeating texture, setting position of the sprite to match the world bounds, and attaching the sprite to the background scene layer
	sf::Texture& snow_texture = m_textures.Get(TextureID::kSnowTile);
	snow_texture.setRepeated(true);
	sf::IntRect snow_texture_rect(0, 0, static_cast<int>(m_world_bounds.width), static_cast<int>(m_world_bounds.height));
	std::unique_ptr<SpriteNode> background_sprite(new SpriteNode(snow_texture, snow_texture_rect));
	background_sprite->setPosition(m_world_bounds.left, m_world_bounds.top);
	m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(background_sprite));

	//Building ice lake
	const float border_thickness = 64.f;
	sf::Texture& icy_lake_texture = m_textures.Get(TextureID::kLakeTile);
	icy_lake_texture.setRepeated(true);
	sf::IntRect lake_texture_rect(0, 0, static_cast<int>(m_world_bounds.width), static_cast<int>(m_world_bounds.height));
	std::unique_ptr<SpriteNode> lake_sprite(new SpriteNode(icy_lake_texture, lake_texture_rect));
	lake_sprite->setScale(0.875, 0.834);
	lake_sprite->setPosition(m_world_bounds.left + border_thickness, m_world_bounds.top + border_thickness);
	m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(lake_sprite));

	sf::FloatRect lake_bounds(m_world_bounds.left + border_thickness, m_world_bounds.top + border_thickness,	m_world_bounds.width * 0.875, m_world_bounds.height * 0.834);
		
	BuildTreesFixed(lake_bounds);
}

//Building trees in a fixed manner - GracieChaudhary
void World::BuildTreesFixed(sf::FloatRect lake_bounds) {

	sf::Texture& green_tree_texture = m_textures.Get(TextureID::kGreenTree);
	const float border_thickness = 64.f;
	// Tree size and spacing ranges
	const float min_tree_size = 32.f;  // Minimum tree size
	const float max_tree_size = 72.f;  // Maximum tree size
	const float min_tree_spacing = -16.f;  // Minimum spacing between trees
	const float max_tree_spacing = 4.f;  // Maximum spacing between trees
	const float max_offset = 10.f;  // Max offset range for natural placement

	std::srand(static_cast<unsigned int>(std::time(0)));

	// Lake boundaries
	float lake_left = lake_bounds.left-10;
	float lake_top = lake_bounds.top-10;
	float lake_right = lake_bounds.left + lake_bounds.width+10;
	float lake_bottom = lake_bounds.top + lake_bounds.height+10;

	//  to generate random offset between -max_offset and +max_offset
	auto randomOffset = [max_offset]() {
		return (std::rand() % (static_cast<int>(max_offset * 2 + 1))) - max_offset;
		};

	// to generate random tree size between min_tree_size and max_tree_size
	auto randomTreeSize = [min_tree_size,max_tree_size]() {
		return min_tree_size + std::rand() % static_cast<int>(max_tree_size - min_tree_size + 1);
		};

	// to generate random tree spacing between min_tree_spacing and max_tree_spacing
	auto randomTreeSpacing = [min_tree_spacing, max_tree_spacing]() {
		return min_tree_spacing + std::rand() % static_cast<int>(max_tree_spacing - min_tree_spacing + 1);
		};

	for (float x = lake_left + randomTreeSpacing(); x <= lake_right; x += (randomTreeSize() + randomTreeSpacing())) {
		float offsetY = randomOffset(); 
		float tree_size = randomTreeSize(); 

		std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(green_tree_texture));
		tree_sprite->setPosition(x, lake_top - border_thickness + offsetY);
		tree_sprite->setScale(tree_size / 64.f, tree_size / 64.f);
		m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
	}

	
	for (float x = lake_left + randomTreeSpacing(); x <= lake_right; x += (randomTreeSize() + randomTreeSpacing())) {
		float offsetY = randomOffset(); 
		float tree_size = randomTreeSize(); 

		std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(green_tree_texture));
		tree_sprite->setPosition(x, lake_bottom + offsetY);
		tree_sprite->setScale(tree_size / 64.f, tree_size / 64.f); 
		m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
	}

	
	for (float y = lake_top - border_thickness; y <= lake_bottom + border_thickness; y += (randomTreeSize() + randomTreeSpacing())) {
		float offsetX = randomOffset(); 
		float tree_size = randomTreeSize(); 

		std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(green_tree_texture));
		tree_sprite->setPosition(lake_left - border_thickness + offsetX, y);
		tree_sprite->setScale(tree_size / 64.f, tree_size / 64.f);
		m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
	}

	
	for (float y = lake_top - border_thickness; y <= lake_bottom + border_thickness; y += (randomTreeSize() + randomTreeSpacing())) {
		float offsetX = randomOffset(); 
		float tree_size = randomTreeSize();  

		std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(green_tree_texture));
		tree_sprite->setPosition(lake_right + offsetX, y);
		tree_sprite->setScale(tree_size / 64.f, tree_size / 64.f); 
		m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
	}	
}


void World::AdaptPlayerPosition()
{
	//keep the player on the screen
	sf::FloatRect view_bounds(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());

	m_character_one->HandleBorderInteraction(view_bounds);

}

void World::AdaptPlayerVelocity()
{

	//m_character_one->HandleSliding();
	//m_character_two->HandleSliding();
	// 
	// 
	////If they are moving diagonally divide by sqrt 2
	//if (velocity.x != 0.f && velocity.y != 0.f)
	//{
	//	m_player_aircraft->SetVelocity(velocity / std::sqrt(2.f));
	//}
}

//Dominik Hampejs D00250604
//Creates random pickup on a random location on the lake
void World::CreatePickup(SceneNode& node, const TextureHolder& textures) const
{
	float border_distance = 65.f;
	auto type = static_cast<PickupType>(Utility::RandomInt(static_cast<int>(PickupType::kPickupCount)));
	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	float x = Utility::RandomInt(GetViewBounds().width - border_distance * 2) + border_distance;
	float y = Utility::RandomInt(GetViewBounds().height - border_distance * 2) + border_distance;
	pickup->setPosition(x,y);
	node.AttachChild(std::move(pickup));
}

sf::FloatRect World::GetViewBounds() const
{
	return sf::FloatRect(m_camera.getCenter() - m_camera.getSize()/2.f, m_camera.getSize());
}

sf::FloatRect World::GetBattleFieldBounds() const
{
	return GetViewBounds();
}

void World::DestroyEntitiesOutsideView()
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


void World::HandleCollisions()
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
			SoundEffect pickupSoundEffect = pickup.GetPickupType() == PickupType::kHealthRefill ? SoundEffect::kHealthPickup: SoundEffect::kSnowballPickup;
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
			character.Accelerate(projectile.GetVelocity() / (3.f,3.f));
			character.PlayLocalSound(m_command_queue, SoundEffect::kSnowballHitPlayer);
			character.Impacted();
			projectile.Destroy();
		}
		
	}
}

void World::UpdateSounds()
{
	// Set listener's position to player position
	m_sounds.SetListenerPosition(m_character_one->GetWorldPosition());

	// Remove unused sounds
	m_sounds.RemoveStoppedSounds();
}

//Dominik Hampejs D00250604
//Create a pickup every time it is time
void World::CheckPickupDrop(sf::Time dt)
{
	// Check if it's time to spawn a new pickup
	if (m_time_since_last_drop > m_pickup_drop_interval)
	{
		m_time_since_last_drop = sf::Time::Zero;
		m_pickups_spawned++;
		m_command_queue.Push(m_create_pickup_command);
	}

	//When no pickup on the lake speed up spawn
	if (m_pickups_spawned == 0)
	{
		m_time_since_last_drop += dt;
	}

	if(m_pickups_spawned < m_max_pickups)
	{
		m_time_since_last_drop += dt;
	}
}
