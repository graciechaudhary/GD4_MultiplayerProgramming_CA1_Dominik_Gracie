#pragma once
#include "PostEffect.hpp"
#include "ResourceIdentifiers.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <array>

class ShadowEffect : public PostEffect
{
public:
    ShadowEffect();

    virtual void Apply(const sf::RenderTexture& scene, sf::RenderTarget& output);

private:
    void PrepareTextures(sf::Vector2u size);

    void GenerateDepthMap(const sf::RenderTexture& scene, const sf::Vector3f& lightPosition);
    void ApplyShadows(const sf::RenderTexture& scene, sf::RenderTarget& output);

private:
    ShaderHolder        m_shaders;

    sf::RenderTexture   m_depth_texture; // Texture to store the depth map
};
