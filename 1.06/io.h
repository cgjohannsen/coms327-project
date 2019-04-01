#ifndef IO_H
# define IO_H

# include "dungeon.h"

# define DUNGEON_X 80
# define DUNGEON_Y 21

# define DISPLAY_ALL_CMD       0
# define DISPLAY_MAP_CMD       1
# define DISPLAY_MONSTERS_CMD  2
# define DISPLAY_TELEPORT_CMD  3

class io {

public:

	io();

	int display(int, dungeon &);	
	int display_all(dungeon &);
	int display_map(dungeon &);
	int display_monsters(dungeon &);
	int display_teleport(dungeon &);

};

#endif