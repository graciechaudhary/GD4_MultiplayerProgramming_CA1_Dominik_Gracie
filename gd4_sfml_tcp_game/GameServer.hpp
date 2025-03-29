#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Config.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Thread.hpp>
#include <map>
#include <memory>
#include <string>
#include <SFML/Graphics/Rect.hpp> 
#include "PlayersController.hpp"
#include "WorldServer.hpp"

class GameServer
{
public:
	GameServer();
	~GameServer();

private:
	struct RemotePeer
	{
		RemotePeer();
		sf::TcpSocket m_socket;
		sf::Time m_last_packet_time;
		sf::Int16 m_identifier;
		bool m_ready;
		bool m_timed_out;
		bool m_game_ready;
		RGBColour m_colour;
		std::string m_name;
	};

	typedef std::unique_ptr<RemotePeer> PeerPtr;

private:
	void SetListening(bool enable);
	void ExecutionThread();
	void Tick();
	sf::Time Now() const;

	void HandleIncomingPackets();
	void HandleIncomingPackets(sf::Packet& packet, RemotePeer& receiving_peer, bool& detected_timeout);

	void HandleIncomingConnections();
	void HandleDisconnections();

	void SendToAll(sf::Packet& packet);

	void BroadcastMessage(const std::string& message);

	void InformWorldState(sf::TcpSocket& socket);

	void NotifyPlayerSpawn(sf::Int16 identifier, sf::Int16 place);

	sf::Int16 GetSpawnPlace();

private:
	sf::Thread m_thread;
	sf::Clock m_clock;
	sf::TcpListener m_listener_socket;
	bool m_listening_state;
	sf::Time m_client_timeout;

	sf::Int16 m_max_connected_players;
	sf::Int16 m_connected_players;

	std::vector<PeerPtr> m_peers;
	bool m_waiting_thread_end;

	WorldServer m_world;
	std::map<sf::Int16, PlayersController*> m_player_controllers;

	bool m_game_started;
	std::deque<sf::Int16> m_places;
};

