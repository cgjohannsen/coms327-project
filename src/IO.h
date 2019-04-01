#ifndef IO_H
# define IO_H

# include "Dungeon.h"

# define DUNGEON_X 80
# define DUNGEON_Y 21

# define DISPLAY_ALL_CMD       0
# define DISPLAY_MAP_CMD       1
# define DISPLAY_MONSTERS_CMD  2
# define DISPLAY_TELEPORT_CMD  3

class IO {

public:

	IO();

	int display(int, Dungeon &);	
	int display_all(Dungeon &);
	int display_map(Dungeon &);
	int display_monsters(Dungeon &);
	int display_teleport(Dungeon &);
	int parse_monsters(Dungeon &);

};

#endif