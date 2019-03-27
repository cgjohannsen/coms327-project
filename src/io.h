#ifndef IO_H
# define IO_H

# include "dungeon.h"

class io {

public:

	io();

	int display(int, dungeon &);	
	int display_all(dungeon &);
	int display_map(dungeon &);
	int display_monsters(dungeon &);

};

#endif