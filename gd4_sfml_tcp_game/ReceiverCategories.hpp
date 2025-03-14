//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
enum class ReceiverCategories
{
	kNone = 0,
	kScene = 1 << 0,
	kPlayer = 1 << 1,
	kProjectile = 1 << 2,
	kPickup = 1 << 3,
	kParticleSystem = 1 << 4,
	kSoundEffect = 1 << 5,
};

// A message would be sent to all aircraft
//unsigned int all_aircraft = ReceiverCategories::kPlayerAircraft | ReceiverCategories::kAlliedAircraft | ReceiverCategories::kEnemyAircraft