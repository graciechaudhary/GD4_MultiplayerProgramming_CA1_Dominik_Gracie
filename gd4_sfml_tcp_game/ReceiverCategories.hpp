#pragma once
enum class ReceiverCategories
{
	kNone = 0,
	kScene = 1 << 0,
	kPlayerOne = 1 << 1,
	kPlayerTwo = 1 << 2,
	kPlayerOneProjectile = 1 << 4,
	kPlayerTwoProjectile = 1 << 5,
	kPickup = 1 << 6,
	kParticleSystem = 1 << 7,
	kSoundEffect = 1 << 8,

	kPlayerCharacter = kPlayerOne  | kPlayerTwo,
	kProjectile = kPlayerOneProjectile | kPlayerTwoProjectile
};

// A message would be sent to all aircraft
//unsigned int all_aircraft = ReceiverCategories::kPlayerAircraft | ReceiverCategories::kAlliedAircraft | ReceiverCategories::kEnemyAircraft