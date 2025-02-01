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
	,m_scrollspeed(-50.f)
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
	m_character_one->ClearWalkingFlags(dt);
	m_character_two->ClearWalkingFlags(dt);

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
	//if (PostEffect::IsSupported())
	//{
	//	m_scene_texture.clear();
	//	m_scene_texture.setView(m_camera);
	//	m_scene_texture.draw(m_scenegraph);
	//	m_scene_texture.display();
	//	m_bloom_effect.Apply(m_scene_texture, m_target);
	//	//m_shadow_effect.Apply(m_scene_texture, m_target);

	//}
	//else
	//{
	//	m_target.setView(m_camera);
	//	m_target.draw(m_scenegraph);
	//}

	m_target.setView(m_camera);
	m_target.draw(m_scenegraph);
}

CommandQueue& World::GetCommandQueue()
{
	return m_command_queue;
}

bool World::HasAlivePlayerOne() const
{
	return !m_character_one->IsMarkedForRemoval();
}

bool World::HasAlivePlayerTwo() const
{
	return !m_character_two->IsMarkedForRemoval();
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
	InitializeLayers();

	//builds snow landsacpe, adds tree sprite - Gracie Chaudhary
	BuildSnowLandscape();	

	//Add the player's aircraft
	std::unique_ptr<Character> leader(new Character(CharacterType::kDefault, m_textures, m_fonts, true));
	m_character_one = leader.get();
	m_character_one->setPosition(m_centre_position.x - 230, m_centre_position.y);
	m_character_one->SetVelocity(0, 0);
	m_scene_layers[static_cast<int>(SceneLayers::kIntreacations)]->AttachChild(std::move(leader));
	

	std::unique_ptr<Character> second(new Character(CharacterType::kDefault, m_textures, m_fonts, false));
	m_character_two = second.get();
	m_character_two->setPosition(m_centre_position.x + 220, m_centre_position.y);
	m_character_two->SetVelocity(0, 0);
	m_scene_layers[static_cast<int>(SceneLayers::kIntreacations)]->AttachChild(std::move(second));

	
	std::unique_ptr<ParticleNode> snowNode(new ParticleNode(ParticleType::kSnowOne, m_textures,true));
	m_scene_layers[static_cast<int>(SceneLayers::kParticles)]->AttachChild(std::move(snowNode));

	std::unique_ptr<ParticleNode> snowNodeTwo(new ParticleNode(ParticleType::kSnowTwo, m_textures, false));
	m_scene_layers[static_cast<int>(SceneLayers::kParticles)]->AttachChild(std::move(snowNodeTwo));


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

	//sf::FloatRect lake_bounds(m_world_bounds.left + 96, m_world_bounds.top + 96,	m_world_bounds.width * 0.8125, m_world_bounds.height * 0.75);
		
	//BuildTreesRandom(lake_bounds);
	//BuildTreesFixed(lake_bounds);
}

//Building trees randomly - GracieChaudhary
void World::BuildTreesRandom(sf::FloatRect lake_bounds) {

	//Placing tree sprites
	sf::Texture& purple_tree_texture = m_textures.Get(TextureID::kPurpleTree);
	sf::Texture& green_tree_texture = m_textures.Get(TextureID::kGreenTree);
	sf::Texture& dead_tree_texture = m_textures.Get(TextureID::kDeadTree);

	/*std::unique_ptr<SpriteNode> purpleTreeSprite(new SpriteNode(purpleTreeTexture));
	std::unique_ptr<SpriteNode> greenTreeSprite(new SpriteNode(greenTreeTexture));
	std::unique_ptr<SpriteNode> deadTreeSprite(new SpriteNode(deadTreeTexture));*/

	
	// Dimensions and border rules
	const int tree_size = 64;
	const sf::Vector2f screen_size(1024, 768);
	const int border_thickness = 96;

	// Define placement bounds
	std::vector<sf::FloatRect> allowed_areas = {
		{0.f, 0.f, screen_size.x, border_thickness},                            // Top border
		{0.f, screen_size.y - border_thickness, screen_size.x, border_thickness}, // Bottom border
		{0.f, border_thickness, border_thickness, screen_size.y - 2 * border_thickness}, // Left border
		{screen_size.x - border_thickness, border_thickness, border_thickness, screen_size.y - 2 * border_thickness} // Right border
	};

	std::set<std::pair<int, int>> occupied_positions;

	auto place_trees = [&](sf::Texture& tree_texture, int count) {
		std::mt19937 rng(static_cast<unsigned>(std::time(nullptr))); // Random number generator

		for (int i = 0; i < count; ++i) {
			std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(tree_texture));

			bool is_valid = false;
			sf::Vector2f position;

			while (!is_valid) {
				// Randomly select one of the allowed areas
				int area_index = rng() % allowed_areas.size();
				const sf::FloatRect& area = allowed_areas[area_index];

				// Generate a random position within the selected area
				std::uniform_real_distribution<float> x_dist(area.left, area.left + area.width - tree_size);
				std::uniform_real_distribution<float> y_dist(area.top, area.top + area.height - tree_size);

				position = { x_dist(rng), y_dist(rng) };

				// Round position to the nearest integer to avoid floating-point imprecision
				std::pair<int, int> rounded_position = {
					static_cast<int>(std::round(position.x / tree_size)) * tree_size,
					static_cast<int>(std::round(position.y / tree_size)) * tree_size
				};

				// Ensure the position is not already occupied and does not overlap the lake
				sf::FloatRect tree_bounds(rounded_position.first, rounded_position.second, tree_size, tree_size);
				if (!lake_bounds.intersects(tree_bounds) && occupied_positions.find(rounded_position) == occupied_positions.end()) {
					is_valid = true;
					occupied_positions.insert(rounded_position);
				}
			}

			// Place the tree
			tree_sprite->setPosition(position);
			m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
		}
		};

	
	place_trees(purple_tree_texture, 15);
	place_trees(green_tree_texture, 15);
	place_trees(dead_tree_texture, 15);

}

//Building trees in a fixed manner - GracieChaudhary
void World::BuildTreesFixed(sf::FloatRect lake_bounds) {

	//Placing tree sprites
	sf::Texture& purple_tree_texture = m_textures.Get(TextureID::kPurpleTree);
	sf::Texture& green_tree_texture = m_textures.Get(TextureID::kGreenTree);
	sf::Texture& dead_tree_texture = m_textures.Get(TextureID::kDeadTree);

	// Tree size and spacing
	const float tree_size = 64.f;
	const float tree_spacing = -24.f; // Negative for overlapping
	const float clearing_gap =32.f; // Distance between forest and lake
	const float border_thickness = 96.f; // Border thickness

	// Lake boundaries
	float lake_left = lake_bounds.left;
	float lake_top = lake_bounds.top;
	float lake_right = lake_bounds.left + lake_bounds.width;
	float lake_bottom = lake_bounds.top + lake_bounds.height;

	// Screen boundaries
	float screen_left = m_world_bounds.left;
	float screen_top = m_world_bounds.top;
	float screen_right = m_world_bounds.left + m_world_bounds.width;
	float screen_bottom = m_world_bounds.top + m_world_bounds.height;

	// Alternating tree placement
	bool place_green_tree = true; // Start with a green tree


	//// Top and bottom rows (fill left to right and right to left)
	//for (float x = lake_left - clearing_gap; x < lake_right + clearing_gap; x += (tree_size + tree_spacing)) {
	//	// Place top row
	//	if (x >= m_world_bounds.left && x <= m_world_bounds.left + m_world_bounds.width) {
	//		std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(place_green_tree ? green_tree_texture : purple_tree_texture));
	//		tree_sprite->setPosition(x, lake_top - clearing_gap);
	//		m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
	//		// Alternate tree type
	//		place_green_tree = !place_green_tree;
	//	}
	//}
	//for (float x = lake_left - clearing_gap; x < lake_right + clearing_gap; x += (tree_size + tree_spacing)) {
	//	// Place bottom row
	//	if (x >= m_world_bounds.left && x <= m_world_bounds.left + m_world_bounds.width) {
	//		std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(place_green_tree ? green_tree_texture : purple_tree_texture));
	//		tree_sprite->setPosition(x, lake_bottom + clearing_gap);
	//		m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
	//		// Alternate tree type
	//		place_green_tree = !place_green_tree;
	//	}
	//}
	//// Left and right columns (fill top to bottom and bottom to top)
	//for (float y = lake_top - clearing_gap; y < lake_bottom + clearing_gap; y += (tree_size + tree_spacing)) {
	//	// Place left column
	//	if (y >= m_world_bounds.top && y <= m_world_bounds.top + m_world_bounds.height) {
	//		std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(place_green_tree ? green_tree_texture : purple_tree_texture));
	//		tree_sprite->setPosition(lake_left - clearing_gap, y);
	//		m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
	//		// Alternate tree type
	//		place_green_tree = !place_green_tree;
	//	}
	//}
	//for (float y = lake_top - clearing_gap; y < lake_bottom + clearing_gap; y += (tree_size + tree_spacing)) {
	//	// Place right column
	//	if (y >= m_world_bounds.top && y <= m_world_bounds.top + m_world_bounds.height) {
	//		std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(place_green_tree ? green_tree_texture : purple_tree_texture));
	//		tree_sprite->setPosition(lake_right + clearing_gap, y);
	//		m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
	//		// Alternate tree type
	//		place_green_tree = !place_green_tree;
	//	}
	//}

	//// Top and bottom rows (multiple rows with overlapping)
	//for (float y = -32; y < lake_top + clearing_gap + (tree_size * 4); y += (tree_size + tree_spacing)) {
	//	for (float x = lake_left - clearing_gap; x < lake_right + clearing_gap; x += (tree_size + tree_spacing)) {
	//		// Place top row of trees
	//		if (x >= m_world_bounds.left && x <= m_world_bounds.left + m_world_bounds.width) {
	//			std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(place_green_tree ? green_tree_texture : purple_tree_texture));
	//			tree_sprite->setPosition(x, y);
	//			m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
	//			// Alternate tree type
	//			place_green_tree = !place_green_tree;
	//		}
	//	}
	//}
	//for (float y = lake_bottom + clearing_gap; y < lake_bottom + clearing_gap + (tree_size * 4); y += (tree_size + tree_spacing)) {
	//	for (float x = lake_left - clearing_gap; x < lake_right + clearing_gap; x += (tree_size + tree_spacing)) {
	//		// Place bottom row of trees
	//		if (x >= m_world_bounds.left && x <= m_world_bounds.left + m_world_bounds.width) {
	//			std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(place_green_tree ? green_tree_texture : purple_tree_texture));
	//			tree_sprite->setPosition(x, y);
	//			m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
	//			// Alternate tree type
	//			place_green_tree = !place_green_tree;
	//		}
	//	}
	//}
	//// Left and right columns (multiple columns with overlapping)
	//for (float x = lake_left - clearing_gap; x < lake_left - clearing_gap + (tree_size * 4); x += (tree_size + tree_spacing)) {
	//	for (float y = lake_top - clearing_gap; y < lake_bottom + clearing_gap; y += (tree_size + tree_spacing)) {
	//		// Place left column of trees
	//		if (y >= m_world_bounds.top && y <= m_world_bounds.top + m_world_bounds.height) {
	//			std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(place_green_tree ? green_tree_texture : purple_tree_texture));
	//			tree_sprite->setPosition(x, y);
	//			m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
	//			// Alternate tree type
	//			place_green_tree = !place_green_tree;
	//		}
	//	}
	//}
	//for (float x = lake_right + clearing_gap; x < lake_right + clearing_gap + (tree_size * 4); x += (tree_size + tree_spacing)) {
	//	for (float y = lake_top - clearing_gap; y < lake_bottom + clearing_gap; y += (tree_size + tree_spacing)) {
	//		// Place right column of trees
	//		if (y >= m_world_bounds.top && y <= m_world_bounds.top + m_world_bounds.height) {
	//			std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(place_green_tree ? green_tree_texture : purple_tree_texture));
	//			tree_sprite->setPosition(x, y);
	//			m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
	//			// Alternate tree type
	//			place_green_tree = !place_green_tree;
	//		}
	//	}
	//}

	// Top and bottom rows (multiple rows with overlapping)
	for (float y = m_world_bounds.top - clearing_gap; y < lake_top - clearing_gap; y += (tree_size + tree_spacing)) {
		for (float x = m_world_bounds.left + border_thickness; x < m_world_bounds.left + m_world_bounds.width - border_thickness; x += (tree_size + tree_spacing)) {
			// Place top row of trees (stop just before the lake)
			std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(place_green_tree ? green_tree_texture : purple_tree_texture));
			tree_sprite->setPosition(x, y);
			m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
			// Alternate tree type
			place_green_tree = !place_green_tree;
		}
	}
	for (float y = m_world_bounds.top + m_world_bounds.height; y > lake_bottom + clearing_gap; y -= (tree_size + tree_spacing)) {
		for (float x = m_world_bounds.left + border_thickness; x < m_world_bounds.left + m_world_bounds.width - border_thickness; x += (tree_size + tree_spacing)) {
			// Place bottom row of trees (stop just before the lake)
			std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(place_green_tree ? green_tree_texture : purple_tree_texture));
			tree_sprite->setPosition(x, y);
			m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
			// Alternate tree type
			place_green_tree = !place_green_tree;
		}
	}
	// Left and right columns (multiple columns with overlapping)
	for (float x = m_world_bounds.left; x < lake_left - clearing_gap; x += (tree_size + tree_spacing)) {
		for (float y = m_world_bounds.top; y < m_world_bounds.top + m_world_bounds.height; y += (tree_size + tree_spacing)) {
			// Place left column of trees (stop just before the lake)
			std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(place_green_tree ? green_tree_texture : purple_tree_texture));
			tree_sprite->setPosition(x, y);
			m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
			// Alternate tree type
			place_green_tree = !place_green_tree;
		}
	}
	for (float x = m_world_bounds.left + m_world_bounds.width; x > lake_right + clearing_gap; x -= (tree_size + tree_spacing)) {
		for (float y = m_world_bounds.top; y < m_world_bounds.top + m_world_bounds.height; y += (tree_size + tree_spacing)) {
			// Place right column of trees (stop just before the lake)
			std::unique_ptr<SpriteNode> tree_sprite(new SpriteNode(place_green_tree ? green_tree_texture : purple_tree_texture));
			tree_sprite->setPosition(x, y);
			m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(tree_sprite));
			// Alternate tree type
			place_green_tree = !place_green_tree;
		}
	}
	
}


void World::AdaptPlayerPosition()
{
	//keep the player on the screen
	sf::FloatRect view_bounds(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());

	m_character_one->HandleBorderInteraction(view_bounds);
	m_character_two->HandleBorderInteraction(view_bounds);

}

void World::AdaptPlayerVelocity()
{

	m_character_one->HandleSliding();
	m_character_two->HandleSliding();
	////If they are moving diagonally divide by sqrt 2
	//if (velocity.x != 0.f && velocity.y != 0.f)
	//{
	//	m_player_aircraft->SetVelocity(velocity / std::sqrt(2.f));
	//}
}

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
		if (MatchesCategories(pair, ReceiverCategories::kPlayerOne, ReceiverCategories::kPlayerTwo))
		{
			auto& player_one = static_cast<Character&>(*pair.first);
			auto& playuer_two = static_cast<Character&>(*pair.second);
			//Collision response
			sf::Vector2f velocity_one = player_one.GetVelocity();
			sf::Vector2f velocity_two = playuer_two.GetVelocity();

			player_one.SetVelocity(0.f, 0.f);
			playuer_two.SetVelocity(0.f, 0.f);

			if (velocity_one == sf::Vector2f(0.f, 0.f))
			{
				velocity_one = -velocity_two;
			}
			if (velocity_two == sf::Vector2f(0.f, 0.f))
			{
				velocity_two = -velocity_one;
			}

			player_one.Accelerate(velocity_two);
			playuer_two.Accelerate(velocity_one);

			player_one.PlayLocalSound(m_command_queue, SoundEffect::kExplosion2);
		}

		else if (MatchesCategories(pair, ReceiverCategories::kPlayerOne, ReceiverCategories::kPickup) || MatchesCategories(pair, ReceiverCategories::kPlayerTwo, ReceiverCategories::kPickup))
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
		else if (MatchesCategories(pair, ReceiverCategories::kPlayerOne, ReceiverCategories::kPlayerTwoProjectile) || MatchesCategories(pair, ReceiverCategories::kPlayerTwo, ReceiverCategories::kPlayerOneProjectile))
		{
			auto& character = static_cast<Character&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);
			//Collision response
			character.Damage(projectile.GetDamage());
			character.SetVelocity(0.f, 0.f);
			character.Accelerate(projectile.GetVelocity() / (3.f,3.f));
			character.PlayLocalSound(m_command_queue, SoundEffect::kSnowballHitPlayer);
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

void World::CheckPickupDrop(sf::Time dt)
{


	// Check if it's time to spawn a new pickup
	if (m_time_since_last_drop > m_pickup_drop_interval)
	{
		m_time_since_last_drop = sf::Time::Zero;
		m_pickups_spawned++;
		m_command_queue.Push(m_create_pickup_command);
	}

	if (m_pickups_spawned == 0)
	{
		m_time_since_last_drop += dt;
	}

	if(m_pickups_spawned < m_max_pickups)
	{
		m_time_since_last_drop += dt;
	}


}
