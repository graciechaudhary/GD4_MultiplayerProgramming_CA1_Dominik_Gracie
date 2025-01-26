#pragma once
#include <SFML/Graphics.hpp>
#include "World.hpp"
#include "GameInputController.hpp"

class Game
{
	public:
		Game();
		void Run();

	private:
		void Update(sf::Time delta_time);
		void Render();
		void ProcessInput();

	private:

		
		static const sf::Time kTimePerFrame;
		sf::RenderWindow m_window;
		World m_world;
		GameInputController m_player;
};