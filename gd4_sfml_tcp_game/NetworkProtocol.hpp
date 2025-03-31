const unsigned short SERVER_PORT = 50000;

const float TICK_RATE = 10.f;
const float FRAME_RATE = 60.f;

const unsigned short MAX_CONNECTIONS = 15;

const float WINDOW_WIDTH = 1920.f;
const float WINDOW_HEIGHT = 1080.f;

namespace Server
{
	enum class PacketType
	{
		kNameSync,					//Name sync - sf::Int16 AMOUNT * (sf::Int16 ID, std::string NAME)
		kBroadcastMessage,			 //Broadcast message - std::string
		kSpawnSelf,					//Spwan self - sf::Int16 ID, sf::Int16 PLACE
		kUpdateClientState,			//TICK - sf::Int16 CHARACTER_AMOUNT * (sf::Int16 ID, float X, float Y, float VX, float VY, sf::Int16 FACINGDIR) + sf::Int16 SNOWBALL_AMOUNT * (sf::Int16 ID, float X, float Y,)
		kHealthUp,					//Health up - sf::Int16 ID
		kHealthDown,				// Health down - sf::Int16 ID
		kInitialState,				//Initial state of the world - sf::Int16 AMOUNT * (sf::Int16 ID, sf::Int16 PLACE, sf::Int16 R, sf::Int16 G, sf::Int16 B)
		kCreateSnowball,			//sf::Int16 ID, sf::Int16 SNOWBALL_ID
		kCharacterRemoved,			//Character removed - sf::Int16 ID
		kSnowballRemoved,			//Snowball removed - sf::Int16 ID
		kPickupSpawned,				//Pickup spawn - sf::Int16 ID, sf::Int16 TYPE, float X, float Y
		kGameReady,					//Game start notice
		kSnowballUp,				//Snowball pickup - sf::Int16 ID
		kWaitingNotice,				//Notice to clients that the game is waiting for players
		kColourSync,				//Shares colour with clients - sf::Int16 ID, sf::Int16 R, sf::Int16 G, sf::Int16 B
		kResults,					//Shares results with clients - sf::Int16 AMOUNT * (sf::Int16 ID, sf::Int16 KILLS, float TIME)
		kHighScores,				//Shares highscores with clients - 5 * (std::string NAME, sf::Int16 KILLS, std::string NAME, float TIME)
	};
}

namespace Client
{
	enum class PacketType
	{
		kRequestNameSync,			// Request for names, also shares your name with the server - std::string
		kBroadcastMessage,			// Broadcast message to all clients - std::string
		kPlayerRealtimeChange,		// Change in player input - sf::Int16, sf::Int16, bool
		kColourChange,				// Change in player colour - sf::Int16, sf::Int16, sf::Int16
		kReadyNotice,				// Ready up notice - sf::Int16
		kNotice,					// Notice to server that client is still alive
		kQuit,						// Quit notice
	};
}