//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "SceneNode.hpp"
#include "CommandQueue.hpp"

class Entity : public SceneNode
{
public:
	Entity(int hitpoints);
	void SetVelocity(sf::Vector2f velocity);
	void SetVelocity(float vx, float vy);
	sf::Vector2f GetVelocity() const;
	void Accelerate(sf::Vector2f velocity);
	void Accelerate(float vx, float vy);

	int GetHitPoints() const;
	void SetHitPoints(int hitpoints);
	void Repair(int points);
	void Repair(int points, int limit);
	virtual void Damage(int points);
	void Destroy();

	virtual bool IsDestroyed() const override;

	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands);
	

private:
	sf::Vector2f m_velocity;
	int m_hitpoints;
};

