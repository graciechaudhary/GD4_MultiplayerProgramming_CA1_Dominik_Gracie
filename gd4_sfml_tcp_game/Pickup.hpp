//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "Entity.hpp"
#include "PickupType.hpp"
#include "ResourceIdentifiers.hpp"

class Character;

class Pickup : public Entity
{
public:
	Pickup(PickupType type, const TextureHolder& textures);
	virtual unsigned int GetCategory() const override;
	virtual sf::FloatRect GetBoundingRect() const;
	void Apply(Character& player) const;
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
	PickupType GetPickupType();

private:
	PickupType m_type;
	sf::Sprite m_sprite;
};

