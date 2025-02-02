//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "SoundPlayer.hpp"

#include "SoundEffect.hpp"

#include <SFML/Audio/Listener.hpp>

#include <cmath>


namespace
{
	// Sound coordinate system, point of view of a player in front of the screen:
	// X = left; Y = up; Z = back (out of the screen)
	const float ListenerZ = 300.f;
	const float Attenuation = 1.f; //how sound drops of
	const float MinDistance2D = 200.f; //min distance at which it will play at full volume
	const float MinDistance3D = std::sqrt(MinDistance2D * MinDistance2D + ListenerZ * ListenerZ);
}

SoundPlayer::SoundPlayer()
{
	//GracieChaudhary - alloting new sound files
	m_sound_buffers.Load(SoundEffect::kExplosion1, "MediaFiles/Sound/ExplosionEffectV1.wav");
	m_sound_buffers.Load(SoundEffect::kExplosion2, "MediaFiles/Sound/ExplosionEffectV2.wav");

	m_sound_buffers.Load(SoundEffect::kSnowballPickup, "MediaFiles/Sound/SnowballPickupEffect.wav");
	m_sound_buffers.Load(SoundEffect::kHealthPickup, "MediaFiles/Sound/HealthPickupEffect.wav");
	m_sound_buffers.Load(SoundEffect::kButton, "MediaFiles/Sound/ButtonSoundEffect1.wav");

	m_sound_buffers.Load(SoundEffect::kSnowballHitPlayer, "MediaFiles/Sound/SnowballHitSoundEffect.ogg");
	m_sound_buffers.Load(SoundEffect::kSnowballThrow, "MediaFiles/Sound/ThrowSoundEffect.ogg");
	

	// Listener points towards the screen (default in SFML)
	sf::Listener::setDirection(0.f, 0.f, -1.f); //pointing towards the screen
}

void SoundPlayer::Play(SoundEffect effect)
{
	Play(effect, GetListenerPosition());
}

void SoundPlayer::Play(SoundEffect effect, sf::Vector2f position)
{
	m_sounds.emplace_back(sf::Sound());
	sf::Sound& sound = m_sounds.back();

	sound.setBuffer(m_sound_buffers.Get(effect));
	sound.setPosition(position.x, -position.y, 0.f); 
	sound.setAttenuation(Attenuation);
	sound.setMinDistance(MinDistance3D);
	sound.setVolume(100.f);

	sound.play();
}

void SoundPlayer::RemoveStoppedSounds()
{
	m_sounds.remove_if([](const sf::Sound& s)
		{
			return s.getStatus() == sf::Sound::Stopped;
		});
}

void SoundPlayer::SetListenerPosition(sf::Vector2f position)
{
	sf::Listener::setPosition(position.x, -position.y, ListenerZ);
}

sf::Vector2f SoundPlayer::GetListenerPosition() const
{
	sf::Vector3f position = sf::Listener::getPosition();
	return sf::Vector2f(position.x, -position.y);
}
