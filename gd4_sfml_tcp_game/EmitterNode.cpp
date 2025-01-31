//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "EmitterNode.hpp"
#include <iostream>

EmitterNode::EmitterNode(ParticleType type)
	:SceneNode()
	, m_accumulated_time(sf::Time::Zero)
	, m_type(type)
	, m_particle_system(nullptr)
{
}

void EmitterNode::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (m_particle_system)
	{
		if (m_type == ParticleType::kSnowExplosion) {
			EmitExplosionParticles(dt);
			//std::cout << "EmitExplosionParticles is being called!" << std::endl;
		}
		else {
			EmitRadialParticles(dt);
			//std::cout << "EmitParticles is being called!" << std::endl;
		}
	}
	
	else
	{
		// Find particle node with the same type as the emitter
		auto finder = [this](ParticleNode& container, sf::Time)
			{
				if (container.GetParticleType() == m_type)
					m_particle_system = &container;
			};
		Command command;
		command.category = static_cast<int>(ReceiverCategories::kParticleSystem);
		command.action = DerivedAction<ParticleNode>(finder);

		commands.Push(command);
	}
}

//void EmitterNode::EmitParticles(sf::Time dt)
//{
//	const float emissionRate = 5.f;
//	const sf::Time interval = sf::seconds(1.f) / emissionRate;
//
//	m_accumulated_time += dt;
//	while (m_accumulated_time > interval)
//	{
//		m_accumulated_time -= interval;
//		m_particle_system->AddParticle(GetWorldPosition());
//	}
//}


//setting new emission pattern - Gracie Chaudhary
void EmitterNode::EmitRadialParticles(sf::Time dt)
{
	const float emissionRate = 50.f; 
	const sf::Time interval = sf::seconds(1.f) / emissionRate;
	m_accumulated_time += dt;

	while (m_accumulated_time > interval) {
		m_accumulated_time -= interval;

		// setting up code for emission in a circular/radial pattern
		float angle = static_cast<float>(std::rand()) / RAND_MAX * 2 * 3.14159f; //converting my random angle in a range between [0,2pi] radians
		float radius = 30.f; // setting distance for the floating particles
		sf::Vector2f offset(radius * std::cos(angle), radius * std::sin(angle)); // seeting x and y coordinates - x = radius*cos(angle), y = radius*sin(angle)

		// Add particle at offset position
		m_particle_system->AddParticle(GetWorldPosition() + offset);
	}
}

void EmitterNode::EmitExplosionParticles(sf::Time dt){

	int num_particles = 30;
	const float minSpeed = 50.f, maxSpeed = 150.f; // Particle speed range
	const float minLifetime = 0.3f, maxLifetime = 1.0f; // Particle lifetime range

	for (int i = 0; i < num_particles; ++i)
	{
		// Generate a random angle in radians
		float angle = static_cast<float>(std::rand()) / RAND_MAX * 2 * 3.14159f;

		// Generate a random speed for variation
		float speed = minSpeed + static_cast<float>(std::rand()) / RAND_MAX * (maxSpeed - minSpeed);

		// Compute velocity based on angle and speed
		sf::Vector2f velocity(speed * std::cos(angle), speed * std::sin(angle));

		// Randomize particle lifetime
		sf::Time lifetime = sf::seconds(minLifetime + static_cast<float>(std::rand()) / RAND_MAX * (maxLifetime - minLifetime));

		// Emit a particle at the given position
		m_particle_system->AddParticle(GetWorldPosition());
	}

}