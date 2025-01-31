#pragma once
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "CharacterAnimationType.hpp";

class CharacterAnimation : public sf::Drawable, public sf::Transformable
{


public:
	CharacterAnimation();
	explicit CharacterAnimation(CharacterAnimationType animation_type,const sf::Texture& texture);

	void SetTexture(const sf::Texture& texture);
	const sf::Texture* GetTexture() const;

	void SetFrameSize(sf::Vector2i frame_size);
	sf::Vector2i GetFrameSize() const;

	void SetNumFrames(std::size_t num_frames);
	std::size_t GetNumFrames() const;

	void SetDuration(sf::Time duration);
	sf::Time GetDuration() const;

	void SetRepeating(bool flag);
	bool IsRepeating() const;

	void Restart();
	bool IsFinished() const;

	sf::FloatRect GetLocalBounds() const;
	sf::FloatRect GetGlobalBounds() const;

	void SetRow(int row);
	sf::IntRect GetCurrentTextureRect() const;

	void Update(sf::Time dt);

	//std::size_t GetCurrentFrame();

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Sprite m_sprite;
	sf::Vector2i m_frame_size;
	std::size_t m_num_frames;
	std::size_t m_current_frame;
	sf::Time m_duration;
	sf::Time m_elapsed_time;
	bool m_repeat;
	int m_row;
	CharacterAnimationType m_type;

};


