//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
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

Projectile::Projectile(ProjectileType type, const TextureHolder& textures, int identifier)
	: Entity(1)
    , m_type(type)
    , m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture), Table[static_cast<int>(type)].m_texture_rect)
	, m_identifier(identifier)
    , m_impact_animation(textures.Get(TextureID::kImpact))
{
    Utility::CentreOrigin(m_sprite);    

    ParticleType particle_type = ParticleType::kSnow;

    //particle system for snow
    std::unique_ptr<EmitterNode> snow(new EmitterNode(particle_type, m_identifier));
    snow->setPosition(0.f, GetBoundingRect().height / 2.f);
    AttachChild(std::move(snow));


    //GracieChaudhary - impact animation for when projectile hits character
    m_impact_animation.SetFrameSize(sf::Vector2i(100, 100));
    m_impact_animation.SetNumFrames(74);
    m_impact_animation.SetDuration(sf::seconds(0.75f));
    Utility::CentreOrigin(m_impact_animation);
    m_impact_animation.setPosition(GetWorldPosition());
    m_impact_animation.scale(0.8f, 0.8f);
    
    
    
}

unsigned int Projectile::GetCategory() const
{
    return static_cast<int>(ReceiverCategories::kProjectile);       
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

bool Projectile::IsMarkedForRemoval() const
{
    return IsDestroyed() && m_impact_animation.IsFinished();
}


void Projectile::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{

    if (IsDestroyed()) {
        SetVelocity(0, 0);
        m_impact_animation.Update(dt);
    }

    Entity::UpdateCurrent(dt, commands);

    
}

void Projectile::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (IsDestroyed())
    {
        target.draw(m_impact_animation, states);
    }
    else
    {
        target.draw(m_sprite, states);

    }
        
}


