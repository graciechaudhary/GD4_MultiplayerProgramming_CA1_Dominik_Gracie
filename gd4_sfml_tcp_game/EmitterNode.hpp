//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "SceneNode.hpp"
#include "ParticleType.hpp"
#include "ParticleNode.hpp"

class EmitterNode : public SceneNode
{
public:
	explicit EmitterNode(ParticleType type, int identifier);
	explicit EmitterNode(ParticleType type, int identifier, ParticleNode* particle_system);
	void VisualUpdate(sf::Time dt);

private:

	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands);
	void EmitRadialParticles(sf::Time dt);	

private:
	sf::Time m_accumulated_time;
	ParticleType m_type;
	ParticleNode* m_particle_system;
	int m_identifier;
};

