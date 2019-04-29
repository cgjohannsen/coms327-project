#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <string>

#include "Dice.h"


int Dice::init(const uint32_t &b, const uint32_t &n, const uint32_t &s)
{
	base = b,
	num = n;
	sides = s;

	return 0;
}

uint32_t Dice::roll()
{
	uint32_t i, sum = base;
	for(i = 0; i < num; i++) {
		sum += (rand() % sides) + 1;
	}

	return sum;
}

char *Dice::c_str()
{
	sprintf(str, "%d+%dd%d", base, num, sides);
	return str;
}

Dice Dice::copy()
{
	Dice temp;
	temp.init(base, num, sides);
	return temp;
}