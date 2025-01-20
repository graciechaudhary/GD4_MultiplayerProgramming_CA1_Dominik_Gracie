#include "ShadowEffect.hpp"
#include "ShaderTypes.hpp"

ShadowEffect::ShadowEffect()
{
    m_shaders.Load(ShaderTypes::kDepthMapGeneration, "MediaFiles/Shaders/Depth.vert", "MediaFiles/Shaders/Depth.frag");
    m_shaders.Load(ShaderTypes::kShadowPass, "Media/Shaders/Fullpass.vert", "MediaFiles/Shaders/Shadow.frag");
}

void ShadowEffect::Apply(const sf::RenderTexture& scene, sf::RenderTarget& output)
{
    // Step 1: Prepare textures (depth map)
    PrepareTextures(scene.getSize());

    // Step 2: Generate depth map
    GenerateDepthMap(scene, sf::Vector3f(0.f, 10.f, 10.f)); // Example light position

    // Step 3: Apply shadows to the scene
    ApplyShadows(scene, output);
}

void ShadowEffect::PrepareTextures(sf::Vector2u size)
{
    // Create a depth texture with the same size as the scene texture
    if (m_depth_texture.getSize() != size)
    {
        m_depth_texture.create(size.x, size.y);
        m_depth_texture.setSmooth(true);
    }
}

void ShadowEffect::GenerateDepthMap(const sf::RenderTexture& scene, const sf::Vector3f& lightPosition)
{
    sf::Shader& depthMapShader = m_shaders.Get(ShaderTypes::kDepthMapGeneration);

    // Set uniforms for depth map generation
    depthMapShader.setUniform("source", scene.getTexture());
    depthMapShader.setUniform("lightPosition", lightPosition);

    // Apply the depth map shader and render to m_depth_texture
    ApplyShader(depthMapShader, m_depth_texture);
}

void ShadowEffect::ApplyShadows(const sf::RenderTexture& scene, sf::RenderTarget& output)
{
    sf::Shader& shadowShader = m_shaders.Get(ShaderTypes::kShadowPass);

    // Set uniforms for shadow pass
    shadowShader.setUniform("source", scene.getTexture());
    shadowShader.setUniform("depthMap", m_depth_texture.getTexture());

    // Apply the shadow shader to the scene and render to output
    ApplyShader(shadowShader, output);
}