//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include <vector>
#include <SFML/System/Time.hpp>
#include "ResourceIdentifiers.hpp"
#include <functional>
#include "Character.hpp"


struct Direction
{
	Direction(float angle, float distance)
		: m_angle(angle), m_distance(distance) {}
	float m_angle;
	float m_distance;
};

struct CharacterData
{
	int m_hitpoints;
	float m_speed;
	TextureID m_texture;
	sf::IntRect m_texture_rect;
	sf::Time m_fire_interval;
	int max_snowballs;
};

struct ProjectileData
{
	int m_damage;
	float m_speed;
	TextureID m_texture;
	sf::IntRect m_texture_rect;
};

struct PickupData
{
	std::function<void(Character&)> m_action;
	TextureID m_texture;
	sf::IntRect m_texture_rect;
};

struct ParticleData
{
	sf::Color m_color;
	sf::Time m_lifetime;
};

struct SpawnPoint
{
	float m_x;
	float m_y;
};

std::vector<CharacterData> InitializeCharacterData();
std::vector<ProjectileData> InitializeProjectileData();
std::vector<PickupData> InitializePickupData();
std::vector<ParticleData> InitializeParticleData();

std::map<int, SpawnPoint> InitializeSpawnPoints();