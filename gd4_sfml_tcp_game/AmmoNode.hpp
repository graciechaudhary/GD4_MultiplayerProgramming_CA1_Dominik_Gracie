#pragma once

#include "SceneNode.hpp"
#include "ResourceIdentifiers.hpp"

class AmmoNode :
    public SceneNode
{
public:
	explicit AmmoNode(const TextureHolder& textures, int ammo_count);
	void SetAmmo(int ammo_count);

private :
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	std::vector<sf::Sprite> m_ammo_sprites;
	int m_ammo_count;
};

