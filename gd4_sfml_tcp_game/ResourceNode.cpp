//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "ResourceNode.hpp"
#include "ResourceHolder.hpp"

//Dominik Hampejs D00250604
ResourceNode::ResourceNode(const TextureHolder& textures,TextureID texture_id , int resource_count, float offset, float scale)
	: m_resource_count(resource_count)
{
	// Load the appropriate texture next to each other based on the resource count with the given offset and scale
	for (size_t i = 0; i < resource_count; i++)
	{
		sf::Sprite sprite(textures.Get(texture_id));
		sprite.setPosition(i * offset, 0.f);
		sprite.scale(scale, scale);
		m_resource_sprites.push_back(sprite);
	}
}
//Dominik Hampejs D00250604
void ResourceNode::SetResource(int resource_count)
{
	m_resource_count = resource_count;
}
//Dominik Hampejs D00250604
void ResourceNode::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	//Draw all resource sprites up to the resource count
	for (int i = 0; i < m_resource_count; i++)
	{
		target.draw(m_resource_sprites[i], states);
	}
}
