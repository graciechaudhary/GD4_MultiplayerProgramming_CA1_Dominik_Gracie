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
, m_game_started(false)
{
	m_listener_socket.setBlocking(false);
	m_peers[0].reset(new RemotePeer());
	m_thread.launch();
	m_places = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
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

    sf::Time frame_rate = sf::seconds(1.f / FRAME_RATE);
    sf::Time frame_time = sf::Time::Zero;
    sf::Time tick_rate = sf::seconds(1.f / TICK_RATE);
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
            if (m_game_started)
            {
                m_world.Update(frame_rate);
                for (auto controler : m_player_controllers)
                {
                    controler.second->NetworkedRealTimeInputServer(m_world.GetCommandQueue());
                }
            }
            frame_time -= frame_rate;
        }

        //Fixed time step
        while (tick_time >= tick_rate)
        {
            if (m_game_started)
            {
                Tick();

                if (m_world.CheckAlivePlayers() == 1)
                {
                    BroadcastMessage("Game Finished");
                }
            }
            else
            {
                sf::Packet packet;
                packet << static_cast<sf::Int16>(Server::PacketType::kWaitingNotice);
                SendToAll(packet);

                sf::Int16 amount_ready = 0;
                for (sf::Int16 i = 0; i < m_connected_players; ++i)
                {
                    if (m_peers[i]->m_game_ready)
                    {
                        amount_ready++;
                    }
                }
				std::cout << amount_ready << " = " << m_connected_players << std::endl; 
                if (amount_ready == m_connected_players)
                {
                    sf::Packet ready_packet;
                    ready_packet << static_cast<sf::Int16>(Server::PacketType::kGameReady);
                    SendToAll(ready_packet);
                    m_game_started = true;
                    SetListening(false);
                    BroadcastMessage("Game Started");
                }
            }

            tick_time -= tick_rate;
        }

        while (!m_world.GetEventQueue().empty()) {
            WorldServer::Packet_Ptr packet;
            packet = std::move(m_world.GetEventQueue().front());
            SendToAll(*packet);
            m_world.GetEventQueue().pop_front();
        }

        //sleep
        //sf::sleep(sf::milliseconds(50));
    }
}

void GameServer::Tick()
{
    /*sf::Packet packet;
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
	}*/

    sf::Packet packet;
    packet << static_cast<sf::Int16>(Server::PacketType::kUpdateClientState);

    sf::Int16 players_alive = m_world.CheckAlivePlayers();
    packet << players_alive;

    for (sf::Int16 i = 0; i < m_connected_players; ++i)
    {
        if (!m_peers[i] || !m_peers[i]->m_ready)
            continue; 

        sf::Int16 identifier = static_cast<sf::Int16>(m_peers[i]->m_identifier);

        auto& characters = m_world.GetCharacters();
        auto it = characters.find(identifier);
        if (it == characters.end())
            continue; 

        Character* character = it->second;

        if (character->IsDestroyed()) continue;

        float x = character->GetWorldPosition().x;
        float y = character->GetWorldPosition().y;
        float vx = character->GetVelocity().x;
        float vy = character->GetVelocity().y;
        sf::Int16 facing_dir = static_cast<sf::Int16>(character->GetFacingDirection());

        packet << identifier << x << y << vx << vy << facing_dir;
    }

    sf::Int16 size = static_cast<sf::Int16>(m_world.GetProjectiles().size());
    packet << size;
	for (auto& projectile : m_world.GetProjectiles())
	{
		sf::Int16 identifier = projectile.first;
		float x = projectile.second->GetWorldPosition().x;
		float y = projectile.second->GetWorldPosition().y;
		
		//packet << identifier << x << y;
        packet << identifier << x << y;
	}

    //pickups
	/*size = static_cast<sf::Int16>(m_world.GetPickups().size());
	packet << size;
	for (auto& pickup : m_world.GetPickups())
	{
		sf::Int16 identifier = pickup.first;
		float x = pickup.second->GetWorldPosition().x;
		float y = pickup.second->GetWorldPosition().y;
		packet << identifier << x << y;
	}*/

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
            break;
        }
        case Client::PacketType::kReadyNotice : {
            sf::Int16 id;
            packet >> id;

            for (sf::Int16 i = 0; i < m_connected_players; ++i)
            {
                if (m_peers[i]->m_identifier == id)
                {
                    m_peers[i]->m_game_ready = m_peers[i]->m_game_ready ? false : true;
                }
            }

        }

        case Client::PacketType::kColourChange: {
			sf::Int16 identifier;
			packet >> identifier;

            sf::Int16 r;
			packet >> r;

			sf::Int16 g;
			packet >> g;

			sf::Int16 b;
			packet >> b;

			m_world.GetCharacter(identifier)->SetColour(sf::Color(r, g, b));
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
        sf::Int16 place = GetSpawnPlace();

		sf::Packet packet;
		packet << static_cast<sf::Int16>(Server::PacketType::kSpawnSelf);
		packet << m_connected_players;
        packet << place;
		m_peers[m_connected_players]->m_socket.send(packet);

        m_peers[m_connected_players]->m_identifier = m_connected_players;
        m_peers[m_connected_players]->m_ready = true;
        m_peers[m_connected_players]->m_last_packet_time = Now();

		m_player_controllers[m_connected_players] = new PlayersController(&m_peers[m_connected_players]->m_socket, m_connected_players);

		m_world.AddCharacter(m_connected_players, place);

        InformWorldState(m_peers[m_connected_players]->m_socket);
        NotifyPlayerSpawn(m_connected_players, place);

		std::cout << "Player " << m_connected_players << " connected at place: " <<  place << std::endl;
        

        m_connected_players++;
        if (m_connected_players >= m_max_connected_players)
        {
            SetListening(false);
        }
        else
        {
            m_peers.emplace_back(PeerPtr(new RemotePeer()));
        }

        BroadcastMessage("Waiting for players");
    }
}

void GameServer::HandleDisconnections()
{
    for (auto itr = m_peers.begin(); itr != m_peers.end();)
    {
        if ((*itr)->m_timed_out)
        {
			sf::Packet packet;
			packet << static_cast<sf::Int16>(Server::PacketType::kCharacterRemoved);
			packet << (*itr)->m_identifier;

            m_places.push_front(m_world.GetCharacter((*itr)->m_identifier)->GetPlace());

			m_world.RemoveCharacter((*itr)->m_identifier);

            m_connected_players--;
            itr = m_peers.erase(itr);

			SendToAll(packet);

            //If the number of peers has dropped below max_connections
            if (m_connected_players < m_max_connected_players && !m_game_started)
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

void GameServer::InformWorldState(sf::TcpSocket& socket)
{
    sf::Packet packet;
    packet << static_cast<sf::Int16>(Server::PacketType::kInitialState);
    packet << m_connected_players;

    for (sf::Int16 i = 0; i < m_connected_players; ++i)
    {
        if (m_peers[i]->m_ready)
        {
            sf::Int16 identifier = m_peers[i]->m_identifier;
			sf::Int16 place = m_world.GetCharacter(identifier)->GetPlace();
			packet << identifier << place;
        }
    }

    socket.send(packet);
}

void GameServer::NotifyPlayerSpawn(sf::Int16 identifier, sf::Int16 place)
{
    sf::Packet packet;
    sf::Int16 size = 1;
    packet << static_cast<sf::Int16>(Server::PacketType::kInitialState);
    packet << size;
    packet << identifier;
	packet << place;

    SendToAll(packet);
}

sf::Int16 GameServer::GetSpawnPlace()
{
	sf::Int16 place = m_places.front();
	m_places.pop_front();
    return place;
}

GameServer::RemotePeer::RemotePeer() : m_ready(false), m_timed_out(false), m_game_ready(false)
{
    m_socket.setBlocking(false);
}
