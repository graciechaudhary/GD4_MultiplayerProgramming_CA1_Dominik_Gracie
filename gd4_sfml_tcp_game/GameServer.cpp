#include "GameServer.hpp"
#include <SFML/Network/Packet.hpp>
#include "NetworkProtocol.hpp"
#include <SFML/System/Sleep.hpp>
#include "Utility.hpp"
#include <iostream>

GameServer::GameServer() : m_thread(&GameServer::ExecutionThread, this)
, m_world()
, m_clock()
, m_listener_socket()
, m_listening_state(false)
, m_client_timeout(sf::seconds(2.f))
, m_max_connected_players(15)
, m_connected_players(0)
, m_peers(1)
, m_waiting_thread_end(false)
{
	m_listener_socket.setBlocking(false);
	m_peers[0].reset(new RemotePeer());
	m_thread.launch();
}

GameServer::~GameServer()
{
	m_waiting_thread_end = true;
	m_thread.wait();
}


void GameServer::SetListening(bool enable)
{
    //Check is the server is already listening
    if (enable)
    {
        if (!m_listening_state)
        {
            m_listening_state = (m_listener_socket.listen(SERVER_PORT) == sf::TcpListener::Done);
        }
    }
    else
    {
        m_listener_socket.close();
        m_listening_state = false;
    }
}

void GameServer::ExecutionThread()
{
    //Initialisation
    SetListening(true);

    sf::Time frame_rate = sf::seconds(1.f / 60.f);
    sf::Time frame_time = sf::Time::Zero;
    sf::Time tick_rate = sf::seconds(1.f / 20.f);
    sf::Time tick_time = sf::Time::Zero;
    sf::Clock frame_clock, tick_clock;

    while (!m_waiting_thread_end)
    {
        //This is the game loop
        HandleIncomingConnections();
        HandleIncomingPackets();

        frame_time += frame_clock.getElapsedTime();
        frame_clock.restart();

        tick_time += tick_clock.getElapsedTime();
        tick_clock.restart();

        //Fixed time step
        while (frame_time >= frame_rate)
        {
			m_world.Update(frame_rate);
            for (auto controler : m_player_controllers)
            {
                controler.second->NetworkedRealTimeInputServer(m_world.GetCommandQueue());
            }
            frame_time -= frame_rate;
        }

        //Fixed time step
        while (tick_time >= tick_rate)
        {
            Tick();
            tick_time -= tick_rate;
        }
        //sleep
        //sf::sleep(sf::milliseconds(50));
    }
}

void GameServer::Tick()
{
	sf::Packet packet;
	packet << static_cast<sf::Int16>(Server::PacketType::kUpdateClientState);
    packet << m_connected_players;
	for (sf::Int16 i = 0; i < m_connected_players; ++i)
	{
		if (m_peers[i]->m_ready)
		{
			sf::Int16 identifier = m_peers[i]->m_identifier;
			float x = m_world.GetCharacter(i)->GetWorldPosition().x;
			float y = m_world.GetCharacter(i)->GetWorldPosition().y;
			float vx = m_world.GetCharacter(i)->GetVelocity().x;
			float vy = m_world.GetCharacter(i)->GetVelocity().y;
            sf::Int16 facing_dir = static_cast<sf::Int16>(m_world.GetCharacter(i)->GetFacingDirection());

			packet << identifier << x << y << vx << vy << facing_dir;


		}
	}

	SendToAll(packet);
}

sf::Time GameServer::Now() const
{
    return m_clock.getElapsedTime();
}

void GameServer::HandleIncomingPackets()
{
    bool detected_timeout = false;

    for (PeerPtr& peer : m_peers)
    {
        if (peer->m_ready)
        {
            sf::Packet packet;
            while (peer->m_socket.receive(packet) == sf::Socket::Done)
            {
                //Interpret the packet and react to it
                HandleIncomingPackets(packet, *peer, detected_timeout);

                peer->m_last_packet_time = Now();
                packet.clear();
            }

            if (Now() > peer->m_last_packet_time + m_client_timeout)
            {
                peer->m_timed_out = true;
                detected_timeout = true;
            }

        }
    }

    if (detected_timeout)
    {
        HandleDisconnections();
    }
}

void GameServer::HandleIncomingPackets(sf::Packet& packet, RemotePeer& receiving_peer, bool& detected_timeout)
{
    sf::Int16 packet_type;
    packet >> packet_type;

    switch (static_cast<Client::PacketType> (packet_type))
    {
	    case Client::PacketType::kBroadcastMessage:
	    {
		    std::string message;
		    packet >> message;
		    BroadcastMessage(message);
		    break;
	    }
        case Client::PacketType::kPlayerRealtimeChange: {
			sf::Int16 identifier;
			sf::Int16 action;
			bool action_enabled;

			packet >> identifier >>action >> action_enabled;
			m_player_controllers[identifier]->RegisterRealTimeInputChange(static_cast<Action>(action), action_enabled);

			BroadcastMessage("Player action change: " + action);
        }
    default:
        break;
    }
}

void GameServer::HandleIncomingConnections()
{
    if (!m_listening_state)
    {
        return;
    }

    if (m_listener_socket.accept(m_peers[m_connected_players]->m_socket) == sf::TcpListener::Done)
    {

		sf::Packet packet;
		packet << static_cast<sf::Int16>(Server::PacketType::kSpawnSelf);
		packet << m_connected_players;
		m_peers[m_connected_players]->m_socket.send(packet);

        m_peers[m_connected_players]->m_identifier = m_connected_players;
        m_peers[m_connected_players]->m_ready = true;
        m_peers[m_connected_players]->m_last_packet_time = Now();

		m_player_controllers[m_connected_players] = new PlayersController(&m_peers[m_connected_players]->m_socket, m_connected_players);
		m_world.AddCharacter(m_connected_players);

        m_connected_players++;
        if (m_connected_players >= m_max_connected_players)
        {
            SetListening(false);
        }
        else
        {
            m_peers.emplace_back(PeerPtr(new RemotePeer()));
        }
    }
}

void GameServer::HandleDisconnections()
{
    for (auto itr = m_peers.begin(); itr != m_peers.end();)
    {
        if ((*itr)->m_timed_out)
        {
            m_connected_players--;
            itr = m_peers.erase(itr);

            //If the number of peers has dropped below max_connections
            if (m_connected_players < m_max_connected_players)
            {
                m_peers.emplace_back(PeerPtr(new RemotePeer()));
                SetListening(true);
            }
        }
        else
        {
            ++itr;
        }
    }
}

void GameServer::SendToAll(sf::Packet& packet)
{
    for (sf::Int16 i = 0; i < m_connected_players; ++i)
    {
        if (m_peers[i]->m_ready)
        {
            m_peers[i]->m_socket.send(packet);
        }
    }
}

void GameServer::BroadcastMessage(const std::string& message)
{
	sf::Packet packet;
	packet << static_cast<sf::Int16>(Server::PacketType::kBroadcastMessage);
	packet << message;
	SendToAll(packet);
}

GameServer::RemotePeer::RemotePeer() : m_ready(false), m_timed_out(false)
{
    m_socket.setBlocking(false);
}
