const unsigned short SERVER_PORT = 50000;

namespace Server
{
	enum class PacketType
	{
		kBroadcastMessage
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