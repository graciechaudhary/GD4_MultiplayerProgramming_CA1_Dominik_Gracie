#include "MultiplayerState.hpp"
#include "Utility.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <fstream>
#include "PickupType.hpp"
#include <iostream>

sf::IpAddress GetAddressFromFile()
{
	{
		//Try to open existing file
		std::ifstream input_file("ip.txt");
		std::string ip_address;
		if (input_file >> ip_address)
		{
			return ip_address;
		}
	}

	//If the open/read failed, create a new file
	std::ofstream output_file("ip.txt");
	std::string local_address = "127.0.0.1";
	output_file << local_address;
	return local_address;

}
MultiplayerState::MultiplayerState(StateStack& stack, Context context, bool is_host)
	:State(stack, context)
	, m_world(*context.window, *context.fonts, *context.sounds)
	, m_window(*context.window)
	, m_texture_holder(*context.textures)
	, m_players_controller(*context.players_controller)
	, m_connected(false)
	, m_game_server(nullptr)
	, m_active_state(true)
	, m_host(is_host)
	, m_client_timeout(sf::seconds(5.f))
	, m_time_since_last_packet(sf::seconds(0.f))
{
	m_broadcast_text.setFont(context.fonts->Get(Font::kMain));
	Utility::CentreOrigin(m_broadcast_text);
	m_broadcast_text.setPosition(m_window.getSize().x / 2.f / 2, 100.f);
	m_failed_connection_text.setFillColor(sf::Color::Black);

	//Use this for "Attempt to connect" and "Failed to connect" messages
	m_failed_connection_text.setFont(context.fonts->Get(Font::kMain));
	m_failed_connection_text.setCharacterSize(35);
	m_failed_connection_text.setFillColor(sf::Color::White);
	m_failed_connection_text.setString("Attempting to connect...");
	Utility::CentreOrigin(m_failed_connection_text);
	m_failed_connection_text.setPosition(m_window.getSize().x / 2.f, m_window.getSize().y / 2.f);

	//Render an establishing connection frame for user feedback
	m_window.clear(sf::Color::Black);
	m_window.draw(m_failed_connection_text);
	m_window.display();
	m_failed_connection_text.setString("Failed to connect to server");
	Utility::CentreOrigin(m_failed_connection_text);

	//If this is the host, create a server
	sf::IpAddress ip;

	if (m_host)
	{
		m_game_server.reset(new GameServer());
		ip = "127.0.0.1";
	}
	else
	{
		ip = GetAddressFromFile();
	}

	if (m_socket.connect(ip, SERVER_PORT, sf::seconds(5.f)) == sf::TcpSocket::Done)
	{
		m_connected = true;
	}
	else
	{
		m_failed_connection_clock.restart();
	}

	//Set socket to non-blocking
	m_socket.setBlocking(false);

	context.music->Play(MusicThemes::kGame);
}

void MultiplayerState::Draw()
{
	m_world.Draw();
	if (m_connected)
	{
		//Show the broadcast message in default view
		m_window.setView(m_window.getDefaultView());

		if (!m_broadcasts.empty())
		{
			m_window.draw(m_broadcast_text);
		}
	}
	else
	{
		m_window.draw(m_failed_connection_text);
	}
}

bool MultiplayerState::Update(sf::Time dt)
{
	if (m_connected)
	{
		m_world.Update(dt);

		//Handle messages from the server that may have arrived
		sf::Packet packet;
		if (m_socket.receive(packet) == sf::Socket::Done)
		{
			m_time_since_last_packet = sf::seconds(0.f);
			sf::Int16 packet_type;
			packet >> packet_type;
			HandlePacket(packet_type, packet);
		}
		else
		{
			//Check for timeout with the server
			if (m_time_since_last_packet > m_client_timeout)
			{
				m_connected = false;
				m_failed_connection_text.setString("Lost connection to the server");
				Utility::CentreOrigin(m_failed_connection_text);

				m_failed_connection_clock.restart();
			}
		}

		if (m_tick_clock.getElapsedTime() > sf::seconds(1.f / TICK_RATE))
		{
			sf::Packet packetOut;
			packetOut << static_cast<sf::Int16>(Client::PacketType::kBroadcastMessage);
			packetOut << "Working!";
			m_socket.send(packetOut);
			m_tick_clock.restart();
		}



		UpdateBroadcastMessage(dt);
		m_time_since_last_packet += dt;
	}

	//Failed to connect and waited for more than 5 seconds: Back to menu
	else if (m_failed_connection_clock.getElapsedTime() >= sf::seconds(5.f))
	{
		RequestStackClear();
		RequestStackPush(StateID::kMenu);
	}

	return true;
}

bool MultiplayerState::HandleEvent(const sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Return && m_connected)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int16>(Client::PacketType::kBroadcastMessage);
			packet << "Hi there!";
			m_socket.send(packet);
		}
	}

	m_players_controller.HandleEvent(event);
    return true;
}

void MultiplayerState::OnActivate()
{
	m_active_state = true;
}

void MultiplayerState::OnDestroy()
{
	if (!m_host && m_connected)
	{
		//Inform server this client is dying
		sf::Packet packet;
		packet << static_cast<sf::Int16>(Client::PacketType::kQuit);
		m_socket.send(packet);
	}
}

void MultiplayerState::UpdateBroadcastMessage(sf::Time elapsed_time)
{
	if (m_broadcasts.empty())
	{
		return;
	}

	//Update broadcast timer
	m_broadcast_elapsed_time += elapsed_time;
	if (m_broadcast_elapsed_time > sf::seconds(2.f))
	{
		//If message has expired, remove it
		m_broadcasts.erase(m_broadcasts.begin());

		//Continue to display the next broadcast message
		if (!m_broadcasts.empty())
		{
			m_broadcast_text.setString(m_broadcasts.front());
			Utility::CentreOrigin(m_broadcast_text);
			m_broadcast_elapsed_time = sf::Time::Zero;
		}
	}
}

void MultiplayerState::HandlePacket(sf::Int16 packet_type, sf::Packet& packet)
{
	switch (static_cast<Server::PacketType>(packet_type))
	{
		//Send message to all Clients
	case Server::PacketType::kBroadcastMessage:
	{
		std::string message;
		packet >> message;
		m_broadcasts.push_back(message);

		//Just added the first message, display immediately
		if (m_broadcasts.size() == 1)
		{
			m_broadcast_text.setString(m_broadcasts.front());
			Utility::CentreOrigin(m_broadcast_text);
			m_broadcast_elapsed_time = sf::Time::Zero;
		}
	}
	break;

		case Server::PacketType::kSpawnSelf:{
			sf::Int16 identifier;
			packet >> identifier;
			m_identifier = identifier;
			m_world.AddCharacter(identifier);
			m_players_controller.SetConnection(&m_socket, identifier);
		}
									   break;
		case Server::PacketType::kInitialState: {
			sf::Int16 amount;
			packet >> amount;
			std::cout << "Chars: " << amount << std::endl;
			for (sf::Int16 i = 0; i < amount; i++)
			{

				sf::Int16 id;
				packet >> id;

				if (id == m_identifier) continue;

				m_world.AddCharacter(id);
			}

									   }
											  break;
		case Server::PacketType::kHealthDown: {
			sf::Int16 identifer;
			packet >> identifer;
			m_world.GetCharacter(identifer)->Damage(1);
		}

		case Server::PacketType::kUpdateClientState: {

			sf::Int16 character_count;
			packet >> character_count;
			for (sf::Int16 i = 0; i < character_count; ++i)
			{
				sf::Int16 character_identifier;
				packet >> character_identifier;
				Character* character = m_world.GetCharacter(character_identifier);
				if (character)
				{
					float x, y;
					packet >> x >> y;
					character->setPosition(x, y);
					
					float vx, vy;
					packet >> vx >> vy;
					character->SetVelocity(vx, vy);

					sf::Int16 dir;
					packet >> dir;
					character->SetCurrentDirection(static_cast<FacingDirections>(dir));
				}
			}
		}
												   break;
	default:
		break;
	}
}
