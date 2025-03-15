//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "DataTables.hpp"
#include "CharacterType.hpp"
#include "ProjectileType.hpp"
#include "PickupType.hpp"
#include "Character.hpp"
#include "ParticleType.hpp"
#include "NetworkProtocol.hpp"

//GracieChaudhary - alloting new textures
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

//GracieChaudhary - alloting new textures
std::vector<ProjectileData> InitializeProjectileData()
{
    std::vector<ProjectileData> data(static_cast<int>(ProjectileType::kProjectileCount));
    data[static_cast<int>(ProjectileType::kSnowball)].m_damage = 1;
    data[static_cast<int>(ProjectileType::kSnowball)].m_speed = 500;
    data[static_cast<int>(ProjectileType::kSnowball)].m_texture = TextureID::kSnowball;

    return data;
}

//GracieChaudhary - alloting new textures
std::vector<PickupData> InitializePickupData()
{
    std::vector<PickupData> data(static_cast<int>(PickupType::kPickupCount));

    data[static_cast<int>(PickupType::kHealthRefill)].m_texture = TextureID::kHealthRefill;
    data[static_cast<int>(PickupType::kHealthRefill)].m_action = [](Character& a)
        {
            a.Repair(1, a.GetMaxHitpoints());
        };

	data[static_cast<int>(PickupType::kSnowballRefill)].m_texture = TextureID::kSnowballRefill;
	data[static_cast<int>(PickupType::kSnowballRefill)].m_action = [](Character& a)
        {
            a.RechargeSnowballs();
        };
    
    return data;
}

//GracieChaudhary - setting up new particle data
std::vector<ParticleData> InitializeParticleData()
{
    std::vector<ParticleData> data(static_cast<int>(ParticleType::kParticleCount));

    data[static_cast<int>(ParticleType::kSnow)].m_color = sf::Color(255, 255, 255);
    data[static_cast<int>(ParticleType::kSnow)].m_lifetime = sf::seconds(1.f);


    return data;
}

std::map<int, SpawnPoint> InitializeSpawnPoints()
{
    std::map<int, SpawnPoint> data;
    float offset_x = 100.f;
    float offset_y = 100.f;

    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        data[i] = SpawnPoint();

        if ( (i/4)%2 == 0)
        {
            switch (i % 4)
            {
            case 0:
                data[i].m_x = offset_x;
                data[i].m_y = offset_y;
                break;
            case 1:
                data[i].m_x = WINDOW_WIDTH - offset_x;
                data[i].m_y = WINDOW_HEIGHT - offset_y;
                break;
            case 2:
                data[i].m_x = offset_x;
                data[i].m_y = WINDOW_HEIGHT - offset_y;
                break;
            case 3:
                data[i].m_x = WINDOW_WIDTH - offset_x;
                data[i].m_y = offset_y;
                offset_x += 150.f;
                offset_y += 100.f;
                break;
            default:
                break;
            }
        }
        else
        {
            switch (i % 4)
            {
            case 0:
                data[i].m_x = WINDOW_WIDTH/2.f;
                data[i].m_y = offset_y;
                break;
            case 1:
                data[i].m_x = WINDOW_WIDTH - offset_x;
                data[i].m_y = WINDOW_HEIGHT/2.f;
                break;
            case 2:
                data[i].m_x = WINDOW_WIDTH / 2.f;
                data[i].m_y = WINDOW_HEIGHT- offset_y;
                break;
            case 3:
                data[i].m_x = offset_x;
                data[i].m_y = WINDOW_HEIGHT / 2.f;
                offset_x += 150.f;
                offset_y += 100.f;
                break;
            default:
                break;
            }
        }
    }

    return data;
}
