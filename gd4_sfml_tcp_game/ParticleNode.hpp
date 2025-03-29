//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "SceneNode.hpp"
#include "ParticleType.hpp"
#include "ResourceIdentifiers.hpp"
#include "Particle.hpp"

class ParticleNode : public SceneNode
{
public:
	ParticleNode(ParticleType type, const TextureHolder& textures, int identifier);

	void AddParticle(sf::Vector2f position);
	ParticleType GetParticleType() const;
	virtual unsigned int GetCategory() const;
	void SetColor(sf::Color color);

	int GetIdentifier() const;


	void UpdateVisuals(sf::Time dt);

private:
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands);
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	void AddVertex(float worldX, float worldY, float texCoordX, float textCoordY, const sf::Color& color) const;
	void ComputeVertices() const;

private:
	std::deque<Particle> m_particles;
	const sf::Texture& m_texture;
	ParticleType m_type;
	sf::Color m_color;

	int m_identifier;

	mutable sf::VertexArray m_vertex_array;
	mutable bool m_needs_vertex_update;

};

