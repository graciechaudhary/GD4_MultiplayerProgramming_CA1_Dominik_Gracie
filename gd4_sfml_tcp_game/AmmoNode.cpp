#include "AmmoNode.hpp"
#include "ResourceHolder.hpp"

AmmoNode::AmmoNode(const TextureHolder& textures, int ammo_count)
{
	for (size_t i = 0; i < ammo_count; i++)
	{
		sf::Sprite sprite(textures.Get(TextureID::kSnowball));
		sprite.setPosition(i * 12.f, 0.f);
		sprite.scale(0.3f, 0.3f);
		m_ammo_sprites.push_back(sprite);
	}
	m_ammo_count = ammo_count;
}

void AmmoNode::SetAmmo(int ammo_count)
{
	m_ammo_count = ammo_count;
}

void AmmoNode::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (int i = 0; i < m_ammo_count; i++)
	{
		target.draw(m_ammo_sprites[i], states);
	}
}
