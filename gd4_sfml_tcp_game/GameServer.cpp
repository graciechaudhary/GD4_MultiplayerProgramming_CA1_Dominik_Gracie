#include "GameServer.hpp"
#include <SFML/Network/Packet.hpp>
#include "NetworkProtocol.hpp"
#include <SFML/System/Sleep.hpp>
#include "Utility.hpp"
#include <iostream>
#include <fstream>

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

	//Dominik
	//A queue of available places for players to spawn that correspond to spawn points from DataTables
	m_places = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };

    {
		//---------------------------------------------Read Highscores from files

        //Try to open existing file
        std::ifstream input_file("kills_highscores.txt");
        for (int i = 0; i < m_max_connected_players; ++i)
        {
            std::string name;
            int kills;
            if (input_file >> name >> kills)
            {
				AddKillScore(kills, name);
            }
            else
            {
				AddKillScore(0, "Error");
            }
        }
    }

    {
        //Try to open existing file
        std::ifstream input_file("time_highscores.txt");
        for (int i = 0; i < m_max_connected_players; ++i)
        {
            std::string name;
            float time;
            if (input_file >> name >> time)
            {
                AddTimeScore(time, name);
            }
            else
            {
                AddTimeScore(0, "Error");
            }
        }
    }


	std::cout << "Kills Highscores" << std::endl;
    for (auto& score : m_high_scores_kills)
    {
        std::cout << score.first << " " << score.second << std::endl;
    }

	std::cout << "Time Highscores" << std::endl;
	for (auto& score : m_high_scores_time)
	{
		std::cout << score.first << " " << score.second << std::endl;
	}
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
            //Dominik
			//------------------Update world and handle input from clients IF game has started
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
				//Dominik
				//---------------------------------Check if game has ended
                if (m_world.CheckAlivePlayers() == 1)
                {
                    BroadcastMessage("Game Finished");

					//Get winner into score board
                    m_world.MarkWinnersScore();

					//Check for highscores
					auto& players = m_world.GetPlayerRecords();
					for (auto& player : players){
						std::string name = m_world.GetCharacter(player.first)->GetName();
						AddKillScore(player.second.m_kills, name);
						AddTimeScore(player.second.m_survival_time.asSeconds(), name);
                    }

					//Write highscores to file
                    std::ofstream output_file_kills("kills_highscores.txt");
					std::ofstream output_file_time("time_highscores.txt");

					//Share highscores with clients
					sf::Packet packet;
					packet << static_cast<sf::Int16>(Server::PacketType::kHighScores);
					for (int i = 0; i < 5; ++i)
					{
						packet << m_high_scores_kills[i].second << m_high_scores_kills[i].first;
						packet << m_high_scores_time[i].second << m_high_scores_time[i].first;

						output_file_kills << m_high_scores_kills[i].second << " " << m_high_scores_kills[i].first << " ";
						output_file_time << m_high_scores_time[i].second << " " << m_high_scores_time[i].first << " ";
					}
					SendToAll(packet);

					output_file_kills.close();
					output_file_time.close();

					//Share score board with clients
                    m_world.PrintRecords();
					m_waiting_thread_end = true;
                }
            }
            else
            {
                //Dominik
				//---------------------------------Check if all players are ready to start the game
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
                if (amount_ready == m_connected_players && amount_ready != 0)
                {
                    sf::Packet ready_packet;
                    ready_packet << static_cast<sf::Int16>(Server::PacketType::kGameReady);
                    SendToAll(ready_packet);
                    m_game_started = true;
                    SetListening(false);
                    BroadcastMessage("Game Started");
					m_world.StartClock();
                }
            }

            tick_time -= tick_rate;
        }

        //Handle events called from the world
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

//Dominik & Gracie
void GameServer::Tick()
{
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
		
        packet << identifier << x << y;
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
    case Client::PacketType::kQuit: {
		receiving_peer.m_timed_out = true;
		detected_timeout = true;
		break;
    }
    case Client::PacketType::kBroadcastMessage:
    {
        std::string message;
        packet >> message;
        BroadcastMessage(message);
        break;
    }
	//Dominik & Gracie
    case Client::PacketType::kPlayerRealtimeChange: {
        sf::Int16 identifier;
        sf::Int16 action;
        bool action_enabled;

        packet >> identifier >> action >> action_enabled;
        m_player_controllers[identifier]->RegisterRealTimeInputChange(static_cast<Action>(action), action_enabled);
        break;
    }
    //Dominik                                              
    case Client::PacketType::kReadyNotice: {
        sf::Int16 id;
        packet >> id;

        for (sf::Int16 i = 0; i < m_connected_players; ++i)
        {
            if (m_peers[i]->m_identifier == id)
            {
                m_peers[i]->m_game_ready = m_peers[i]->m_game_ready ? false : true;
            }
        }
        break;
    }
    //Gracie
    case Client::PacketType::kColourChange: {

        sf::Int16 r;
		packet >> r;

		sf::Int16 g;
		packet >> g;

		sf::Int16 b;
		packet >> b;

		receiving_peer.m_colour = RGBColour(r, g, b);
			

        sf::Packet colour_change;
		colour_change << static_cast<sf::Int16>(Server::PacketType::kColourSync);
		colour_change << receiving_peer.m_identifier << r << g << b;
		SendToAll(colour_change);
		break;
    }
    //Gracie
    case Client::PacketType::kRequestNameSync: {
		std::string name;
		packet >> name;
		receiving_peer.m_name = name;
		m_world.GetCharacter(receiving_peer.m_identifier)->SetName(name);

		sf::Packet name_packet;
		name_packet << static_cast<sf::Int16>(Server::PacketType::kNameSync);
		name_packet << m_connected_players;

        for (auto& peer : m_peers) {
			if (peer->m_ready) {
				name_packet << peer->m_identifier << peer->m_name;
			}
        }
		receiving_peer.m_socket.send(name_packet);

        sf::Packet new_name;
		new_name << static_cast<sf::Int16>(Server::PacketType::kNameSync);
		sf::Int16 size = 1;
        new_name << size;
		new_name << receiving_peer.m_identifier << receiving_peer.m_name;
		SendToAll(new_name);
		break;
    }
    default:
        break;
    }
}

//Dominik & Gracie
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
		packet << m_player_id_count;
        packet << place;
		m_peers[m_connected_players]->m_socket.send(packet);

        m_peers[m_connected_players]->m_identifier = m_player_id_count;
        m_peers[m_connected_players]->m_ready = true;
        m_peers[m_connected_players]->m_last_packet_time = Now();

		m_player_controllers[m_player_id_count] = new PlayersController(&m_peers[m_connected_players]->m_socket, m_player_id_count);

		m_world.AddCharacter(m_player_id_count, place);

        InformWorldState(m_peers[m_connected_players]->m_socket);
        NotifyPlayerSpawn(m_player_id_count, place);

		std::cout << "Player " << m_player_id_count << " connected at place: " <<  place << std::endl;
        

        m_connected_players++;
		m_player_id_count++;
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

//Dominik & Gracie
void GameServer::HandleDisconnections()
{
    for (auto itr = m_peers.begin(); itr != m_peers.end();)
    {
        if ((*itr)->m_timed_out)
        {
			//Inform all clients of the disconnection
			sf::Packet packet;
			packet << static_cast<sf::Int16>(Server::PacketType::kCharacterRemoved);
			packet << (*itr)->m_identifier;

			//Add place back to the list
            m_places.push_front(m_world.GetCharacter((*itr)->m_identifier)->GetPlace());

			//Remove character from world
			m_world.RemoveCharacter((*itr)->m_identifier, m_game_started);
			m_player_controllers[(*itr)->m_identifier]->SetActive(false);

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

//Dominik & Gracie
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
            sf::Int16 r,g,b;
			r = m_peers[i]->m_colour.GetRed();
			g = m_peers[i]->m_colour.GetGreen();
			b = m_peers[i]->m_colour.GetBlue();
			packet << identifier << place << r << g << b;
		}
	}
	socket.send(packet);
}

//Dominik & Gracie
void GameServer::NotifyPlayerSpawn(sf::Int16 identifier, sf::Int16 place)
{
    sf::Packet packet;
    sf::Int16 size = 1;
    packet << static_cast<sf::Int16>(Server::PacketType::kInitialState);
    packet << size;
    packet << identifier;
	packet << place;
    sf::Int16 r, g, b;
    r = m_peers[identifier]->m_colour.GetRed();
    g = m_peers[identifier]->m_colour.GetGreen();
    b = m_peers[identifier]->m_colour.GetBlue();
	packet << r << g << b;

    SendToAll(packet);
}

//Dominik
sf::Int16 GameServer::GetSpawnPlace()
{
	sf::Int16 place = m_places.front();
	m_places.pop_front();
    return place;
}

//Dominink
void GameServer::AddKillScore(sf::Int16 kills, std::string name)
{
    //Add
    //Sort
    //Pop over 5
    m_high_scores_kills.emplace_back(kills, name);

    std::sort(m_high_scores_kills.begin(), m_high_scores_kills.end(), [](const auto& a, const auto& b) {
        return a.first > b.first;
        });

    if (m_high_scores_kills.size() > 5) {
        m_high_scores_kills.pop_back();
    }
}

//Dominik
void GameServer::AddTimeScore(float time, std::string name)
{
    //Add
	//Sort
	//Pop over 5
    m_high_scores_time.emplace_back(time, name);

    std::sort(m_high_scores_time.begin(), m_high_scores_time.end(), [](const auto& a, const auto& b) {
        return a.first > b.first;
        });

    if (m_high_scores_time.size() > 5) {
        m_high_scores_time.pop_back();
    }

}

GameServer::RemotePeer::RemotePeer() : m_ready(false), m_timed_out(false), m_game_ready(false), m_name("")
{
    m_socket.setBlocking(false);
}
