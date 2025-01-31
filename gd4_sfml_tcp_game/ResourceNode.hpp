//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once

#include "SceneNode.hpp"
#include "ResourceIdentifiers.hpp"

class ResourceNode :
    public SceneNode
{
public:
	explicit ResourceNode(const TextureHolder& textures, TextureID texture_id, int ammo_count, float offset, float scale);
	void SetResource(int resource_count);

private :
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	std::vector<sf::Sprite> m_resource_sprites;
	int m_resource_count;
};

