//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "ParticleNode.hpp"
#include "DataTables.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace
{
    const std::vector<ParticleData> Table = InitializeParticleData();
}

ParticleNode::ParticleNode(ParticleType type, const TextureHolder& textures, int identifier)
    : SceneNode()
    , m_texture(textures.Get(TextureID::kParticle))
    , m_type(type)
    , m_vertex_array(sf::Quads)
    /*fixed particle system glitch: as we were using four vertices in the ComputeVertices method but a triangular vertex array the fourth vertex 
    would attach to the first vertex of the next particle which was fixed with using quads - GracieChaudhary*/
    , m_needs_vertex_update(true)
	, m_identifier(identifier)
{
	m_color = Table[static_cast<int>(type)].m_color;
}

void ParticleNode::AddParticle(sf::Vector2f position)
{
    Particle particle;
    particle.m_position = position;
    particle.m_color = m_color;
    particle.m_lifetime = Table[static_cast<int>(m_type)].m_lifetime;

    m_particles.emplace_back(particle);
}

ParticleType ParticleNode::GetParticleType() const
{
    return m_type;
}

unsigned int ParticleNode::GetCategory() const
{
    return static_cast<int>(ReceiverCategories::kParticleSystem);
}

void ParticleNode::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
    //Remove expired particles at beginning
    while (!m_particles.empty() && m_particles.front().m_lifetime <= sf::Time::Zero)
    {
        m_particles.pop_front();
    }

    //Decrease lifetime of existing particles
    for (Particle& particle : m_particles)
    {
        particle.m_lifetime -= dt;
    }
    m_needs_vertex_update = true;
}

void ParticleNode::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (m_needs_vertex_update)
    {
        ComputeVertices();
        m_needs_vertex_update = false;
    }

    //Apply particle texture
    states.texture = &m_texture;

    //Draw the vertices
    target.draw(m_vertex_array, states);
}

void ParticleNode::AddVertex(float worldX, float worldY, float texCoordX, float texCoordY, const sf::Color& color) const
{
    sf::Vertex vertex;
    vertex.position = sf::Vector2f(worldX, worldY);
    vertex.texCoords = sf::Vector2f(texCoordX, texCoordY);
    vertex.color = color;

    m_vertex_array.append(vertex);
}

//adjusting size according to snow particle - GracieChaudhary
void ParticleNode::ComputeVertices() const
{
    m_vertex_array.clear();
    
    sf::Vector2f size(m_texture.getSize());
    sf::Vector2f half = size / 4.f;     
   
    for (const Particle& particle : m_particles)
    {
        sf::Vector2f pos = particle.m_position;
        sf::Color color = particle.m_color;

        float ratio = particle.m_lifetime.asSeconds() / Table[static_cast<int>(m_type)].m_lifetime.asSeconds();
        color.a = static_cast<sf::Uint8>(255 * std::max(ratio, 0.f));

        AddVertex(pos.x - half.x, pos.y - half.y, 0.f, 0.f, color);
        AddVertex(pos.x + half.x, pos.y - half.y, size.x, 0.f, color);
        AddVertex(pos.x + half.x, pos.y + half.y, size.x, size.y, color);
        AddVertex(pos.x - half.x, pos.y + half.y, 0.f, size.y, color);
    }  

}

//Dominik Hampejs D00250604
void ParticleNode::SetColor(sf::Color color)
{
	m_color = color;
}

//Dominik Hampejs D00250604
int ParticleNode::GetIdentifier() const
{
    return m_identifier;
}

void ParticleNode::UpdateVisuals(sf::Time dt)
{
    while (!m_particles.empty() && m_particles.front().m_lifetime <= sf::Time::Zero)
    {
        m_particles.pop_front();
    }

    //Decrease lifetime of existing particles
    for (Particle& particle : m_particles)
    {
        particle.m_lifetime -= dt;
    }
    m_needs_vertex_update = true;
}


