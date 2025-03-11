const unsigned short SERVER_PORT = 50000;

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