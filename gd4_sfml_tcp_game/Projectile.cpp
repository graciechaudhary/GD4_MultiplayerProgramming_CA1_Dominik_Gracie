#include "Projectile.hpp"
#include "DataTables.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"
#include "EmitterNode.hpp"
#include "ParticleType.hpp"

namespace
{
    const std::vector<ProjectileData> Table = InitializeProjectileData();
}

Projectile::Projectile(ProjectileType type, const TextureHolder& textures, bool is_player_one)
	: Entity(1), m_type(type), m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture), Table[static_cast<int>(type)].m_texture_rect), m_is_player_one(is_player_one)
{
    Utility::CentreOrigin(m_sprite);    

	ParticleType particle_type = is_player_one ? ParticleType::kSnowOne : ParticleType::kSnowTwo;

    //particle system for snow
    std::unique_ptr<EmitterNode> snow(new EmitterNode(particle_type));
    snow->setPosition(0.f, GetBoundingRect().height / 2.f);
    AttachChild(std::move(snow));
}

unsigned int Projectile::GetCategory() const
{
    if (m_is_player_one)
    {
        return static_cast<int>(ReceiverCategories::kPlayerOneProjectile);
        
    }
    else
        return static_cast<int>(ReceiverCategories::kPlayerTwoProjectile);
        
}
 
sf::FloatRect Projectile::GetBoundingRect() const
{
    return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

float Projectile::GetMaxSpeed() const
{
    return Table[static_cast<int>(m_type)].m_speed;
}

float Projectile::GetDamage() const
{
    return Table[static_cast<int>(m_type)].m_damage;
}

void Projectile::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
    Entity::UpdateCurrent(dt, commands);
}

void Projectile::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_sprite, states);
}
