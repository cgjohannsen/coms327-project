#include <cstdlib>
#include <ctime>
#include <string>

#include "Dice.h"


int Dice::init(int &b, int &n, int &s)
{
	base = b,
	num = n;
	sides = s;

	return 0;
}

int Dice::roll()
{
	srand(time(NULL));

	int i, sum = base;
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