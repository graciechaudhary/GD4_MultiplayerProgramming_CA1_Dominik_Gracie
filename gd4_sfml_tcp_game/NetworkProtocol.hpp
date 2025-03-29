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
		kNameSync,
		kBroadcastMessage,
		kSpawnSelf,
		kUpdateClientState,
		kHealthUp,
		kHealthDown,
		kInitialState,
		kCreateSnowball,
		kCharacterRemoved,
		kSnowballRemoved,
		kPickupSpawned,
		kGameReady,
		kSnowballUp,
		kWaitingNotice
	};
}

namespace Client
{
	enum class PacketType
	{
		kRequestNameSync,
		kBroadcastMessage,
		kPlayerRealtimeChange,
		kColourChange,
		kReadyNotice,
		kNotice,
		kQuit
	};
}