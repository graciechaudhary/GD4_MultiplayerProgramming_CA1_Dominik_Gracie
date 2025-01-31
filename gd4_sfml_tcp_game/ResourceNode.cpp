//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "ResourceNode.hpp"
#include "ResourceHolder.hpp"

ResourceNode::ResourceNode(const TextureHolder& textures,TextureID texture_id , int ammo_count, float offset, float scale)
	: m_resource_count(ammo_count)
{
	for (size_t i = 0; i < ammo_count; i++)
	{
		sf::Sprite sprite(textures.Get(texture_id));
		sprite.setPosition(i * offset, 0.f);
		sprite.scale(scale, scale);
		m_resource_sprites.push_back(sprite);
	}
}

void ResourceNode::SetResource(int resource_count)
{
	m_resource_count = resource_count;
}

void ResourceNode::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (int i = 0; i < m_resource_count; i++)
	{
		target.draw(m_resource_sprites[i], states);
	}
}
