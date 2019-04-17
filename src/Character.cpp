#include <cstddef>
#include <cmath>

#include <float.h>

#include "Character.h"
#include "Dungeon.h"

Character::~Character()
{
	
}

int Character::can_see(Dungeon &d, Character &viewer, Character &viewee)
{

	return 1;
}

uint32_t Character::attack()
{
	return attack_damage.roll();
}