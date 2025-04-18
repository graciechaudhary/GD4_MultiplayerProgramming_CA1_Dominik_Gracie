//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "Animation.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

Animation::Animation()
    :m_num_frames(0)
    , m_current_frame(0)
    , m_duration(sf::Time::Zero)
    , m_elapsed_time(sf::Time::Zero)
    , m_repeat(false)
{
}

Animation::Animation(const sf::Texture& texture)
    :m_sprite(texture)
    , m_frame_size()
    , m_num_frames(0)
    , m_current_frame(0)
    , m_duration(sf::Time::Zero)
    , m_elapsed_time(sf::Time::Zero)
    , m_repeat(false)

{
}

void Animation::SetTexture(const sf::Texture& texture)
{
    m_sprite.setTexture(texture);
}

const sf::Texture* Animation::GetTexture() const
{
    return m_sprite.getTexture();
}

void Animation::SetFrameSize(sf::Vector2i frame_size)
{ 
    m_frame_size = frame_size;
}

sf::Vector2i Animation::GetFrameSize() const
{
    return m_frame_size;
}

void Animation::SetNumFrames(std::size_t num_frames)
{
    m_num_frames = num_frames;
}

std::size_t Animation::GetNumFrames() const
{
    return m_num_frames;
}

void Animation::SetDuration(sf::Time duration)
{
    m_duration = duration;
}

sf::Time Animation::GetDuration() const
{
    return m_duration;
}

void Animation::SetRepeating(bool flag)
{
    m_repeat = flag;
}

bool Animation::IsRepeating() const
{
    return m_repeat;
}

void Animation::Restart()
{
    m_current_frame = 0;
}

bool Animation::IsFinished() const
{
    return m_current_frame >= m_num_frames;
}

sf::FloatRect Animation::GetLocalBounds() const
{
    return sf::FloatRect(getOrigin(), static_cast<sf::Vector2f>(GetFrameSize()));
}

sf::FloatRect Animation::GetGlobalBounds() const
{
    return getTransform().transformRect(GetLocalBounds());
}

void Animation::Update(sf::Time dt)
{
    sf::Time time_per_frame = m_duration / static_cast<float>(m_num_frames);
    m_elapsed_time += dt;

    sf::Vector2i textureBounds(m_sprite.getTexture()->getSize());
    sf::IntRect textureRect = m_sprite.getTextureRect();

    if (m_current_frame == 0)
    {
        textureRect = sf::IntRect(0, 0, m_frame_size.x, m_frame_size.y);
    }
    //while we have a frame to process
    while (m_elapsed_time >= time_per_frame && (m_current_frame <= m_num_frames || m_repeat))
    {
        //Move the texture rect left
        textureRect.left += textureRect.width;

        //If we reach the end of the texture
        if (textureRect.left + textureRect.width > textureBounds.x)
        {
            //move it down a line
            textureRect.left = 0;
            textureRect.top += textureRect.height;
        }

        //progress to the next frame
        m_elapsed_time -= time_per_frame;
        if (m_repeat)
        {
            m_current_frame = (m_current_frame + 1) % m_num_frames;
            if (m_current_frame == 0)
            {
                textureRect = sf::IntRect(0, 0, m_frame_size.x, m_frame_size.y);

            }
        }
        else
        {
            m_current_frame++;
        }
    }
    m_sprite.setTextureRect(textureRect);
}

std::size_t Animation::GetCurrentFrame()
{
    return m_current_frame;
}

void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_sprite, states);
}


