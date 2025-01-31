//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include <SFML/Graphics.hpp>
#include "World.hpp"
#include "PlayersController.hpp"

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
		PlayersController m_player;
};