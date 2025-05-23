//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "Pickup.hpp"
#include "DataTables.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"


namespace
{
    const std::vector<PickupData> Table = InitializePickupData();
}

Pickup::Pickup(PickupType type, const TextureHolder& textures)
    : Entity(1)
    , m_type(type)
    , m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture), Table[static_cast<int>(type)].m_texture_rect)
{
    Utility::CentreOrigin(m_sprite);
}

Pickup::Pickup(int16_t identifer, PickupType type, const TextureHolder& textures)
    : Entity(1)
	, m_identifier(identifer)
    , m_type(type)
    , m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture), Table[static_cast<int>(type)].m_texture_rect)
{
    Utility::CentreOrigin(m_sprite);
}

unsigned int Pickup::GetCategory() const
{
    return static_cast<int>(ReceiverCategories::kPickup);
}

sf::FloatRect Pickup::GetBoundingRect() const
{
    return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

void Pickup::Apply(Character& player) const
{
    Table[static_cast<int>(m_type)].m_action(player);
}

void Pickup::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_sprite, states);
}

PickupType Pickup::GetPickupType()
{
    return m_type;
}

sf::Int16 Pickup::GetIdentifier() const
{
    return m_identifier;
}

//isMaarekdForRemoval() override
bool Pickup::IsMarkedForRemoval() const
{
	return IsDestroyed();
}


