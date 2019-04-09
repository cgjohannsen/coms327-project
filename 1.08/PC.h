#ifndef PC_H
# define PC_H

# include <cstdint>

# include "Character.h"

typedef struct heap heap_t;

class Dungeon;

class PC : public Character{

public:

	PC();

	int init(uint8_t, uint8_t);
	int move(Dungeon &, int c, heap_t *h);

};

#endif
