#include "WorldClient.hpp"
#include "Projectile.hpp"
#include "ParticleNode.hpp"
#include "SoundNode.hpp"
#include "EmitterNode.hpp"
#include "SpriteNode.hpp"
#include "DataTables.hpp"
#include <iostream>

namespace {
	std::map<int, SpawnPoint> Table = InitializeSpawnPoints();
}

WorldClient::WorldClient(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds)
	:m_target(output_target)
	//, m_camera(output_target.getDefaultView())
	, m_textures()
	, m_fonts(font)
	, m_sounds(sounds)
	, m_scenegraph(ReceiverCategories::kNone)
	, m_scene_layers()
	//, m_world_bounds(0.f, 0.f, m_camera.getSize().x, m_camera.getSize().y)
	//, m_centre_position(m_camera.getSize().x / 2.f, m_camera.getSize().y / 2.f)
	, m_world_bounds(0.f, 0.f, output_target.getSize().x, output_target.getSize().y)
	, m_centre_position(m_world_bounds.width / 2.f, m_world_bounds.height / 2.f)
	, m_projectile_test(nullptr)
{
	m_scene_texture.create(m_target.getSize().x, m_target.getSize().y);
	LoadTextures();
	BuildScene();
	//m_camera.setCenter(m_centre_position);

	m_create_pickup_command.category = static_cast<int>(ReceiverCategories::kScene);
	m_create_pickup_command.action = [this](SceneNode& node, sf::Time)
		{
			//CreatePickup(node, m_textures);
		};

}

void WorldClient::Draw()
{
	if (PostEffect::IsSupported())
	{
		m_scene_texture.clear();
		//m_scene_texture.setView(m_camera);
		m_scene_texture.draw(m_scenegraph);
		m_scene_texture.display();
		m_bloom_effect.Apply(m_scene_texture, m_target);
	}
	else
	{
		m_target.setView(sf::View(sf::FloatRect(0.f, 0.f, m_world_bounds.width, m_world_bounds.height)));
		//m_target.setView(m_camera);
		m_target.draw(m_scenegraph);
	}
}

void WorldClient::LoadTextures()
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

void WorldClient::BuildScene()
{
	//GracieChaudhary - cleaning the code
	InitializeLayers();

	//builds snow landsacpe, adds tree sprite - Gracie Chaudhary
	BuildSnowLandscape();

	//Add the player's aircraft


	/*std::unique_ptr<Character> second(new Character(false, 2, m_textures, m_fonts));
	m_character_two = second.get();
	m_character_two->setPosition(m_centre_position.x + 220, m_centre_position.y);
	m_character_two->SetVelocity(0, 0);
	m_scene_layers[static_cast<int>(SceneLayers::kIntreacations)]->AttachChild(std::move(second));*/


	std::unique_ptr<ParticleNode> snowNode(new ParticleNode(ParticleType::kSnow, m_textures, 1));
	m_snow_particle = snowNode.get();
	m_scene_layers[static_cast<int>(SceneLayers::kParticles)]->AttachChild(std::move(snowNode));

	// Add sound effect node
	std::unique_ptr<SoundNode> soundNode(new SoundNode(m_sounds));
	m_scenegraph.AttachChild(std::move(soundNode));
}

void WorldClient::InitializeLayers()
{
	for (std::size_t i = 0; i < static_cast<int>(SceneLayers::kLayerCount); ++i)
	{
		ReceiverCategories category = (i == static_cast<int>(SceneLayers::kParticles)) ? ReceiverCategories::kScene : ReceiverCategories::kNone;
		SceneNode::Ptr layer(new SceneNode(category));
		m_scene_layers[i] = layer.get();
		m_scenegraph.AttachChild(std::move(layer));
	}
}

void WorldClient::BuildSnowLandscape()
{
	//const float screen_width = m_world_bounds.width;
	//const float screen_height = m_world_bounds.height;

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
	lake_sprite->setScale((m_world_bounds.width-2*border_thickness)/ m_world_bounds.width, (m_world_bounds.height - 2 * border_thickness) / m_world_bounds.height);
	//
	lake_sprite->setPosition(m_world_bounds.left + border_thickness, m_world_bounds.top + border_thickness);
	m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(lake_sprite));

	sf::FloatRect lake_bounds(m_world_bounds.left + border_thickness, m_world_bounds.top + border_thickness, m_world_bounds.width - 2 * border_thickness, m_world_bounds.height - 2 * border_thickness);

	BuildTreesFixed(lake_bounds);
}

void WorldClient::BuildTreesFixed(sf::FloatRect lake_bounds)
{
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
	float lake_left = lake_bounds.left - 10;
	float lake_top = lake_bounds.top - 10;
	float lake_right = lake_bounds.left + lake_bounds.width + 10;
	float lake_bottom = lake_bounds.top + lake_bounds.height + 10;

	//  to generate random offset between -max_offset and +max_offset
	auto randomOffset = [max_offset]() {
		return (std::rand() % (static_cast<int>(max_offset * 2 + 1))) - max_offset;
		};

	// to generate random tree size between min_tree_size and max_tree_size
	auto randomTreeSize = [min_tree_size, max_tree_size]() {
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


//RecievePickupData from server method



void WorldClient::CreatePickup(SceneNode& node, PickupSpawnPoint& spawnpoint, const TextureHolder& textures) const
{
	std::unique_ptr<Pickup> pickup(new Pickup(spawnpoint.m_type, textures));
	pickup->setPosition(spawnpoint.m_x, spawnpoint.m_y);
	node.AttachChild(std::move(pickup));
}

//sf::FloatRect WorldClient::GetViewBounds() const
//{
//	return sf::FloatRect(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());
//}

sf::FloatRect WorldClient::GetBattleFieldBounds() const
{
	return m_world_bounds;
}

void WorldClient::Update(sf::Time dt)
{
	for (auto charMap : m_characters)
	{
		if (charMap.second == nullptr) return;

		charMap.second->UpdateVisuals(dt);
	}
	for (auto projectile : m_projectiles)
	{
		projectile.second->UpdateVisuals(dt);
	}
	for (auto parSys: m_particle_systems)
	{
		parSys.second->UpdateVisuals(dt);
	}
}

void WorldClient::AddCharacter(sf::Int16 identifier)
{
	std::unique_ptr<Character> leader(new Character(false, identifier, m_textures, m_fonts));
	Character* character = leader.get();
	character->setPosition(Table[identifier].m_x, Table[identifier].m_y);
	character->SetVelocity(0, 0);
	character->SetColour(sf::Color::Yellow);
	m_scene_layers[static_cast<int>(SceneLayers::kIntreacations)]->AttachChild(std::move(leader));

	m_characters[identifier] = character;

	std::unique_ptr<ParticleNode> snowNode(new ParticleNode(ParticleType::kSnow, m_textures, identifier));
	
	m_particle_systems[identifier] = snowNode.get();

	m_scene_layers[static_cast<int>(SceneLayers::kParticles)]->AttachChild(std::move(snowNode));
}

Character* WorldClient::GetCharacter(sf::Int16 identifier)
{
	return m_characters[identifier];
}

Projectile* WorldClient::GetProjectile(sf::Int16 identifier)
{
	return m_projectiles[identifier];
}

void WorldClient::RemoveCharacter(sf::Int16 character_id)
{
	//m_characters.erase(character_id);
	//auto it = m_characters.find(character_id);
	//if (it != m_characters.end()) {
	//	delete it->second; 
	//	m_characters.erase(it);
	//}
}

void WorldClient::RemoveSnowball(sf::Int16 snowball_id)
{
	m_projectiles[snowball_id]->Destroy();
}

void WorldClient::CreateSnowball(sf::Int16 character_identifier, sf::Int16 snowball_identifier)
{

	std::unique_ptr<Projectile> projectile(new Projectile(ProjectileType::kSnowball, m_textures, character_identifier, false, m_particle_systems[character_identifier]));
	m_projectiles[snowball_identifier] = projectile.get();
	std::cout << "Snowball: " << snowball_identifier << std::endl;
	GetCharacter(character_identifier)->CreateSnowball(*m_scene_layers[static_cast<int>(SceneLayers::kParticles)], std::move(projectile));
}

void WorldClient::UpdateSounds()
{
	//m_sounds.SetListenerPosition(m_character->GetWorldPosition());

	// Remove unused sounds
	m_sounds.RemoveStoppedSounds();
}



	


