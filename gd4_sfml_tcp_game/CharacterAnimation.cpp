#include "CharacterAnimation.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

CharacterAnimation::CharacterAnimation()
    :m_num_frames(0)
    , m_current_frame(0)
    , m_duration(sf::Time::Zero)
    , m_elapsed_time(sf::Time::Zero)
    , m_repeat(true)
    , m_row(0)
{

}

CharacterAnimation::CharacterAnimation(CharacterAnimationType animation_type, const sf::Texture& texture)
    :m_sprite(texture)
    , m_frame_size()
    , m_num_frames(0)
    , m_current_frame(0)
    , m_duration(sf::Time::Zero)
    , m_elapsed_time(sf::Time::Zero)
    , m_repeat(true)
    , m_type(animation_type)
    , m_row(0)
{
}

void CharacterAnimation::SetTexture(const sf::Texture& texture)
{
    m_sprite.setTexture(texture);
}

const sf::Texture* CharacterAnimation::GetTexture() const
{
    return m_sprite.getTexture();
}

void CharacterAnimation::SetFrameSize(sf::Vector2i frame_size)
{
    m_frame_size = frame_size;
}

sf::Vector2i CharacterAnimation::GetFrameSize() const
{
    return m_frame_size;
}

void CharacterAnimation::SetNumFrames(std::size_t num_frames)
{
    m_num_frames = num_frames;
}

std::size_t CharacterAnimation::GetNumFrames() const
{
    return m_num_frames;
}

void CharacterAnimation::SetDuration(sf::Time duration)
{
    m_duration = duration;
}

sf::Time CharacterAnimation::GetDuration() const
{
    return m_duration;
}

void CharacterAnimation::SetRepeating(bool flag)
{
    m_repeat = flag;
}

bool CharacterAnimation::IsRepeating() const
{
    return m_repeat;
}

void CharacterAnimation::Restart()
{
    m_current_frame = 0;
}

bool CharacterAnimation::IsFinished() const
{
    return m_current_frame >= m_num_frames;
}

sf::FloatRect CharacterAnimation::GetLocalBounds() const
{
    return sf::FloatRect(getOrigin(), static_cast<sf::Vector2f>(GetFrameSize()));
}

sf::FloatRect CharacterAnimation::GetGlobalBounds() const
{
    return getTransform().transformRect(GetLocalBounds());
}

void CharacterAnimation::Update(sf::Time dt)
{

    sf::Time time_per_frame = m_duration / static_cast<float>(m_num_frames);
    m_elapsed_time += dt;

    sf::Vector2i textureBounds(m_sprite.getTexture()->getSize());
    sf::IntRect textureRect = m_sprite.getTextureRect();

    // Set the row based on movement direction
    textureRect.top = m_row * m_frame_size.y;

    if (m_elapsed_time >= time_per_frame)
    {
        m_current_frame = (m_current_frame + 1) % m_num_frames;
        textureRect.left = m_current_frame * m_frame_size.x;
        m_elapsed_time -= time_per_frame;
    }

    m_sprite.setTextureRect(textureRect);


}

void CharacterAnimation::SetRow(int row)
{
    m_row = row;  // New function to update the current row (direction)
}

sf::IntRect CharacterAnimation::GetCurrentTextureRect() const
{
    return sf::IntRect(
        m_current_frame * m_frame_size.x,  
        m_row * m_frame_size.y,            
        m_frame_size.x,                    
        m_frame_size.y                     
    );

}

void CharacterAnimation::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_sprite, states);
}

std::size_t CharacterAnimation::GetCurrentFrame()
{
    return m_current_frame;
}