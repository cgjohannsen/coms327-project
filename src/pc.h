#ifndef PC_H
# define PC_H

# include <cstdint>

# include "character.h"

typedef struct heap heap_t;

class dungeon;

class pc : public character{

public:

	pc();

	int init(uint8_t, uint8_t);
	int pc_move(dungeon &, int c, heap_t *h);

};

#endif
