#pragma once
#include "State.hpp"
#include "World.hpp"
#include "GameServer.hpp"
#include "NetworkProtocol.hpp"

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
	void HandlePacket(sf::Int32 packet_type, sf::Packet& packet);

private:
	sf::RenderWindow& m_window;
	TextureHolder& m_texture_holder;

	std::size_t m_identifier;
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
};

