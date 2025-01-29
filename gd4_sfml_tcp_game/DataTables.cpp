#include "DataTables.hpp"
#include "CharacterType.hpp"
#include "ProjectileType.hpp"
#include "PickupType.hpp"
#include "Character.hpp"
#include "ParticleType.hpp"

std::vector<CharacterData> InitializeCharacterData()
{
    std::vector<CharacterData> data(static_cast<int>(CharacterType::kCharacterCount));

    data[static_cast<int>(CharacterType::kDefault)].m_hitpoints = 3;
    data[static_cast<int>(CharacterType::kDefault)].m_speed = 200.f;
    data[static_cast<int>(CharacterType::kDefault)].m_fire_interval = sf::seconds(1);
	data[static_cast<int>(CharacterType::kDefault)].max_snowballs = 5;
    data[static_cast<int>(CharacterType::kDefault)].m_texture = TextureID::kCharacterMovement;
    data[static_cast<int>(CharacterType::kDefault)].m_texture_rect = sf::IntRect(0, 0, 38, 42);

    return data;
}


std::vector<ProjectileData> InitializeProjectileData()
{
    std::vector<ProjectileData> data(static_cast<int>(ProjectileType::kProjectileCount));
    data[static_cast<int>(ProjectileType::kSnowball)].m_damage = 1;
    data[static_cast<int>(ProjectileType::kSnowball)].m_speed = 500;
    data[static_cast<int>(ProjectileType::kSnowball)].m_texture = TextureID::kSnowball;

    return data;
}

std::vector<PickupData> InitializePickupData()
{
    std::vector<PickupData> data(static_cast<int>(PickupType::kPickupCount));

    data[static_cast<int>(PickupType::kHealthRefill)].m_texture = TextureID::kEntities;
    data[static_cast<int>(PickupType::kHealthRefill)].m_texture_rect = sf::IntRect(0, 64, 40, 40);
    data[static_cast<int>(PickupType::kHealthRefill)].m_action = [](Character& a)
        {
            a.Repair(1, a.GetMaxHitpoints());
        };

	data[static_cast<int>(PickupType::kSnowballRefill)].m_texture = TextureID::kEntities;
	data[static_cast<int>(PickupType::kSnowballRefill)].m_texture_rect = sf::IntRect(40, 64, 40, 40);
    data[static_cast<int>(PickupType::kSnowballRefill)].m_action = [](Character& a)
        {
            a.RechargeSnowballs();
        };
    
    return data;
}

//setting up new particle data - GracieChaudhary
std::vector<ParticleData> InitializeParticleData()
{
    std::vector<ParticleData> data(static_cast<int>(ParticleType::kParticleCount));

    data[static_cast<int>(ParticleType::kSnow)].m_color = sf::Color(255,255,255);
    data[static_cast<int>(ParticleType::kSnow)].m_lifetime = sf::seconds(1.f);

    return data;
}
