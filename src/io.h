#ifndef IO_H
# define IO_H

# include "dungeon.h"

class io {

public:

	io();

	int display_monsters(dungeon &);
	int display_map(dungeon &);

};

#endif