#include "MultiplayerState.hpp"
#include "Utility.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/IpAddress.hpp>

#include <fstream>
#include "PickupType.hpp"
#include <iostream>

//Dominik
//Get IP and Nametag from file
std::pair<sf::IpAddress, std::string> GetIpAndNametagFromFile()
{
	{
		//Try to open existing file
		std::ifstream input_file("env_info.txt");
		std::string ip_address, nametag;
		if (input_file >> ip_address >> nametag)
		{
			return std::pair<sf::IpAddress, std::string>(ip_address, nametag);
		}
	}

	//If the open/read failed, create a new file
	std::ofstream output_file("env_info.txt");
	std::string local_address = "127.0.0.1";
	std::string nametag = "Tag";
	output_file << local_address << " " << nametag;
	return std::pair<sf::IpAddress, std::string>(local_address, nametag);

}
//Dominik & Gracie
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
	, m_game_started(false)
	, m_player_dead(false)
	, m_gui_container(true)
	, m_is_player_ready(false)
	, m_colour(std::make_unique<RGBColour>())
	, m_game_ended(false)
	, m_game_end_time(sf::seconds(1))

{

	SetUpColourSelectionUI(context);
	m_broadcast_text.setFont(context.fonts->Get(Font::kMain));
	m_broadcast_text.setCharacterSize(50);
	m_broadcast_text.setFillColor(sf::Color::Black);
	Utility::CentreOrigin(m_broadcast_text);
	m_broadcast_text.setPosition(m_window.getSize().x / 2.f, m_window.getSize().y / 2.f);

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

	auto file_info = GetIpAndNametagFromFile();

	ip = file_info.first;
	m_players_controller.SetName(file_info.second);

	if (m_host)
	{
		m_game_server.reset(new GameServer());
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

		if (!m_game_started)
		{
			m_window.draw(m_gui_container);
		}
	}
	else
	{
		m_window.draw(m_failed_connection_text);
	}
}
//Dominik & Gracie
bool MultiplayerState::Update(sf::Time dt)
{
	//When end of game wait for a few seconds before transitioning to the game over state
	if (m_game_ended)
	{
		m_world.Update(dt);

		if (m_game_end_time <= sf::Time::Zero)
		{
			RequestStackPop();
			RequestStackPush(StateID::kGameOver);
		}
		else
		{
			m_game_end_time -= dt;
		}
	}
	else if (m_connected)
	{
		if (m_game_started)
		{
			m_world.Update(dt);
		}


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
			//Send a notice to the server that this client is still alive
			sf::Packet packetOut;
			packetOut << static_cast<sf::Int16>(Client::PacketType::kNotice);
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

//Dominik & Gracie
bool MultiplayerState::HandleEvent(const sf::Event& event)
{
	//Call quit on window close
	if (event.type == sf::Event::Closed)
	{
		sf::Packet packet;
		packet << static_cast<sf::Int16>(Client::PacketType::kQuit);
		m_socket.send(packet);
	}
	

	if (m_game_started && !m_player_dead)
	{
		m_players_controller.HandleEvent(event);
	}

	//Allow colour selection if the game has not started
	if (m_connected && !m_game_started) {
		bool is_colour_selecting = false;

		for (int i = 0; i < 3; i++)
		{
			is_colour_selecting = m_buttons[i]->IsActive();

			//If the player one is selecting their colour, allow them to change it
			if (is_colour_selecting)
			{
				if (event.type == sf::Event::KeyPressed) {
					//Pressing W or S will deactivate the button
					if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::S)
					{
						m_buttons[i]->Deactivate();

						sf::Packet packet;
						packet << static_cast<sf::Int16>(Client::PacketType::kColourChange);

						packet << static_cast<sf::Int16>(m_colour->GetRed());
						packet << static_cast<sf::Int16>(m_colour->GetGreen());
						packet << static_cast<sf::Int16>(m_colour->GetBlue());
						m_socket.send(packet);
					}
					int add = 0;
					//Pressing D or A will change the colour
					if (event.key.code == sf::Keyboard::D)
					{
						add = 20;
					}
					else if (event.key.code == sf::Keyboard::A)
					{
						add = -20;
					}
					//Change the colour based on the button pressed
					switch (i)
					{
					case 0:
						m_colour->addRed(add);
						m_buttons[i]->SetText(std::to_string(m_colour->GetRed()));
						break;
					case 1:
						m_colour->addGreen(add);
						m_buttons[i]->SetText(std::to_string(m_colour->GetGreen()));
						break;
					case 2:
						m_colour->addBlue(add);
						m_buttons[i]->SetText(std::to_string(m_colour->GetBlue()));
						break;
					default:
						break;
					}
				}
				m_world.GetCharacter(m_identifier)->SetColour(m_colour->GetColour());
				m_world.GetParticleSystem(m_identifier)->SetColor(m_colour->GetColour());
			}
		}

		m_gui_container.HandleEvent(event);
	}

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

//Dominik & Gracie
void MultiplayerState::HandlePacket(sf::Int16 packet_type, sf::Packet& packet)
{
	switch (static_cast<Server::PacketType>(packet_type))
	{
		//Everyoone is ready, start the game
	case Server::PacketType::kGameReady: {
		m_game_started = true;
		break;
	}
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
		break;
	}
	//Server has sent the client their identifier and spawn place
	case Server::PacketType::kSpawnSelf:{
		sf::Int16 identifier, place;
		packet >> identifier >> place;
		m_identifier = identifier;
		m_world.AddCharacter(identifier, place, m_players_controller.GetName());
		m_players_controller.SetConnection(&m_socket, identifier);

		//Share players name with the server
		sf::Packet name_packet;
		name_packet << static_cast<sf::Int16>(Client::PacketType::kRequestNameSync);
		name_packet << m_players_controller.GetName();

		m_socket.send(name_packet);

		if (identifier == m_identifier)
		{
			m_world.GetCharacter(identifier)->SetName(m_players_controller.GetName());
		}
		m_world.PlaySoundEffect(identifier, SoundEffect::kExplosion2);

		break;
	}
	//Dominik
	case Server::PacketType::kNameSync: {
		sf::Int16 amount;
		packet >> amount;

		for (sf::Int16 i = 0; i < amount; i++)
		{
			sf::Int16 id;
			std::string name;
			packet >> id >> name;

			if (id == m_identifier) continue;

			m_world.GetCharacter(id)->SetName(name);
		}

		break;
	}

	case Server::PacketType::kInitialState: {
		sf::Int16 amount;
		packet >> amount;
		std::cout << "Chars: " << amount << std::endl;
		for (sf::Int16 i = 0; i < amount; i++)
		{

			sf::Int16 id, place;
			packet >> id >> place;

			sf::Int16 r, g, b;
			packet >> r >> g >> b;

			if (id == m_identifier) continue;

			m_world.AddCharacter(id, place, std::to_string(id));
			m_world.GetCharacter(id)->SetColour(sf::Color(r, g, b));
			m_world.GetParticleSystem(id)->SetColor(sf::Color(r, g, b));
		}
		break;
	}
	case Server::PacketType::kHealthDown: {
		sf::Int16 identifer;
		packet >> identifer;
		m_world.GetCharacter(identifer)->Damage(1);

		if (m_world.GetCharacter(m_identifier)->IsDestroyed())
		{
			m_player_dead = true;
			m_world.PlaySoundEffect(identifer, SoundEffect::kExplosion1);
		}

		if (identifer == m_identifier)
		m_world.PlaySoundEffect(identifer, SoundEffect::kSnowballHitPlayer);
		break;

		
	}
	case Server::PacketType::kHealthUp: {
		sf::Int16 character_identifer;
		sf::Int16 pickup_identifier;
		packet >> character_identifer;
		packet >> pickup_identifier;
		m_world.GetCharacter(character_identifer)->Repair(1, m_world.GetCharacter(character_identifer)->GetMaxHitpoints());
		m_world.RemovePickup(pickup_identifier);

		if (character_identifer == m_identifier)
		m_world.PlaySoundEffect(character_identifer, SoundEffect::kHealthPickup);

		break;

		
	}
	case Server::PacketType::kSnowballUp: {
		sf::Int16 character_identifer;
		sf::Int16 pickup_identifier;
		packet >> character_identifer;
		packet >> pickup_identifier;
		m_world.GetCharacter(character_identifer)->RechargeSnowballs();
		m_world.RemovePickup(pickup_identifier);

		if (character_identifer == m_identifier)
		m_world.PlaySoundEffect(character_identifer, SoundEffect::kSnowballPickup);

		break;


	}
	case Server::PacketType::kCreateSnowball: {
		sf::Int16 identifer;
		sf::Int16 snowball_identifier;
		packet >> identifer >> snowball_identifier;

		m_world.CreateSnowball(identifer, snowball_identifier);

		if (identifer == m_identifier)
		m_world.PlaySoundEffect(identifer, SoundEffect::kSnowballThrow);

		break;
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
					//vx *= 0.5;
					//vy *= 0.5;
					character->SetVelocity(vx, vy);

					sf::Int16 dir;
					packet >> dir;
					character->SetCurrentDirection(static_cast<FacingDirections>(dir));
				}
			}
		sf::Int16 snowball_counter;
		packet >> snowball_counter;
		for (sf::Int16 i = 0; i < snowball_counter; ++i) {
			sf::Int16 id;
			packet >> id;
			float x, y;
			packet >> x >> y;
			m_world.GetProjectile(id)->setPosition(x,y);
		}
		break;
	}
	case Server::PacketType::kCharacterRemoved: {
		sf::Int16 character_id;
		packet >> character_id;

		m_world.RemoveCharacter(character_id);
		
		break;
	}
	case Server::PacketType::kSnowballRemoved: {
		sf::Int16 snowball_id;
		packet >> snowball_id;

		m_world.RemoveSnowball(snowball_id);
		break;
	}
	case Server::PacketType::kPickupSpawned: {
		sf::Int16 pickup_identifier;
		sf::Int16 type;
		float x, y;
		packet >> pickup_identifier >> type >> x >> y;
		m_world.SpawnPickup(pickup_identifier, static_cast<PickupType>(type), x, y);
		break;
	}

	case Server::PacketType::kColourSync: {
		sf::Int16 identifier;
		sf::Int16 red, green, blue;
		packet >> identifier >> red >> green >> blue;
		m_world.GetCharacter(identifier)->SetColour(sf::Color(red, green, blue));
		m_world.GetParticleSystem(identifier)->SetColor(sf::Color(red, green, blue));
		break;
	}
	case Server::PacketType::kResults: {
		std::stringstream& results = m_players_controller.m_score_ss;

		sf::Int16 amount;
		packet >> amount;
		for (sf::Int16 i = 0; i < amount; i++)
		{
			sf::Int16 id;
			float time;
			sf::Int16 kills;
			packet >> id >> time >> kills;

			std::string name = m_world.GetCharacter(id)->GetName();

			results << i+1 << ".\t\t" << name << "\t\tTime: " << time << "s\t\tKills: " << kills << std::endl;
		}

		m_game_ended = true;
		break;
	}
	case Server::PacketType::kHighScores: {
		std::stringstream& kill_scores = m_players_controller.m_kills_score_ss;
		std::stringstream& time_scores = m_players_controller.m_time_score_ss;
		for (int i = 0; i < 5; ++i)
		{
			std::string name;
			sf::Int16 score;
			packet >> name >> score;
			kill_scores << i + 1 << ".\t\t" << name << "\t\t" << score << std::endl;

			float time;
			packet >> name >> time;
			time_scores << i + 1 << ".\t\t" << name << "\t\t" << time << std::endl;
		}

		std::cout << kill_scores.str() << std::endl;
		std::cout << time_scores.str() << std::endl;

		break;
	}


	default:
		break;
	}
}
//Dominik & Gracie
void MultiplayerState::SetUpColourSelectionUI(Context context)
{
	std::string color_text = "220";

	auto red_button = std::make_shared<gui::Button>(context);
	//setting button position to the middle of the screen
	red_button->setPosition(m_window.getSize().x / 2.f - 60, m_window.getSize().y / 2.f - 80);
	red_button->SetText(color_text);
	red_button->SetToggle(true);

	auto green_button = std::make_shared<gui::Button>(context);
	green_button->setPosition(m_window.getSize().x / 2.f - 60, m_window.getSize().y / 2.f - 20);
	green_button->SetText(color_text);
	green_button->SetToggle(true);

	auto blue_button = std::make_shared<gui::Button>(context);
	blue_button->setPosition(m_window.getSize().x / 2.f - 60, m_window.getSize().y / 2.f +40);
	blue_button->SetText(color_text);
	blue_button->SetToggle(true);

	auto ready_button = std::make_shared<gui::Button>(context);
	ready_button->setPosition(m_window.getSize().x / 2.f - 60, m_window.getSize().y / 2.f + 100);
	ready_button->SetText("Confirm");
	ready_button->SetCallback([this]() {
				sf::Packet packet;
				packet << static_cast<sf::Int16>(Client::PacketType::kReadyNotice);
				packet << m_identifier;
				m_socket.send(packet);

				std::string text;
				if (m_is_player_ready)
				{
					m_is_player_ready = false;
					text = "Confirm";
				}
				else {
					text = "Ready";
					m_is_player_ready = true;
					
				}
				m_buttons[3]->SetText(text);
			});
	

	m_buttons.push_back(red_button);
	m_buttons.push_back(green_button);
	m_buttons.push_back(blue_button);
	m_buttons.push_back(ready_button);

	for (auto& button : m_buttons) {
		m_gui_container.Pack(button);
	}

	//lables
	auto red_label = std::make_shared<gui::Label>("Red", *context.fonts);
	red_label->SetOutlineDesign(sf::Color::White, 2);
	red_label->setPosition(m_window.getSize().x / 2.f - 130, m_window.getSize().y / 2.f - 75);
	red_label->SetColor(sf::Color::Red);
	red_label->SetSize(30);

	auto green_label = std::make_shared<gui::Label>("Green", *context.fonts);
	green_label->SetOutlineDesign(sf::Color::White, 2);
	green_label->setPosition(m_window.getSize().x / 2.f - 130, m_window.getSize().y / 2.f -15);
	green_label->SetColor(sf::Color::Green);
	green_label->SetSize(30);

	auto blue_label = std::make_shared<gui::Label>("Blue", *context.fonts);
	blue_label->SetOutlineDesign(sf::Color::White, 2);
	blue_label->setPosition(m_window.getSize().x / 2.f - 130, m_window.getSize().y / 2.f + 35);
	blue_label->SetColor(sf::Color::Blue);
	blue_label->SetSize(30);

	auto title_label = std::make_shared<gui::Label>("Select your player colour", *context.fonts);
	title_label->SetOutlineDesign(sf::Color::White, 2);
	title_label->setPosition(m_window.getSize().x / 2.f - 120, m_window.getSize().y / 2.f - 160);
	title_label->SetColor(sf::Color::Red);
	title_label->SetSize(50);

	m_gui_container.Pack(red_label);
	m_gui_container.Pack(green_label);
	m_gui_container.Pack(blue_label);
	m_gui_container.Pack(title_label);
}
