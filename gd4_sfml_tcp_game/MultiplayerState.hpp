#pragma once
#include "State.hpp"
#include "GameServer.hpp"
#include "NetworkProtocol.hpp"
#include "WorldClient.hpp"
#include "PlayersController.hpp"

#include "Container.hpp"
#include "Button.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include "RGBColour.hpp"
#include "Label.hpp"
#include "ResourceIdentifiers.hpp"

class MultiplayerState : public State
{
public:
	MultiplayerState(StateStack& stack, Context context, bool is_host);
	virtual void Draw();
	virtual bool Update(sf::Time dt);
	virtual bool HandleEvent(const sf::Event& event);
	virtual void OnActivate();
	void OnDestroy();

private:
	void UpdateBroadcastMessage(sf::Time elapsed_time);
	void HandlePacket(sf::Int16 packet_type, sf::Packet& packet);
	void SetUpColourSelectionUI(Context context);

private:
	sf::RenderWindow& m_window;
	TextureHolder& m_texture_holder;
	WorldClient m_world;

	sf::Int16 m_identifier;
	sf::TcpSocket m_socket;
	bool m_connected;
	std::unique_ptr<GameServer> m_game_server;
	sf::Clock m_tick_clock;

	std::vector<std::string> m_broadcasts;
	sf::Text m_broadcast_text;
	sf::Time m_broadcast_elapsed_time;

	sf::Text m_failed_connection_text;
	sf::Clock m_failed_connection_clock;

	bool m_active_state;
	bool m_host;
	sf::Time m_client_timeout;
	sf::Time m_time_since_last_packet;

	PlayersController& m_players_controller;

	bool m_game_started;
	bool m_player_dead;

	gui::Container m_gui_container;
	std::vector<gui::Button::Ptr> m_buttons;
	std::unique_ptr<RGBColour> m_colour;
	bool m_is_player_ready;
	sf::Sprite m_sprite;
	int m_selected_button;
};

