const unsigned short SERVER_PORT = 50000;
const unsigned short WINDOW_WIDTH = 1920;
const unsigned short WINDOW_HEIGHT = 1080;

namespace Server
{
	enum class PacketType
	{
		kBroadcastMessage,
		kSpawnSelf,
		kUpdateClientState,
	};
}

namespace Client
{
	enum class PacketType
	{
		kBroadcastMessage,
		kPlayerRealtimeChange,
		kQuit
	};
}