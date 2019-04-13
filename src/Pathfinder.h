#ifndef PATHFINDER_H
# define PATHFINDER_H

# include <cstdint>
# include "heap.h"

# define DUNGEON_X 80
# define DUNGEON_Y 21
#define row       1
#define col       0

class Pathfinder{

public:

	typedef struct dungeon_path{
		heap_node_t *hn;
 		uint8_t pos[2];
 		uint8_t from[2];
 		uint16_t cost;
	} dungeon_path_t;

	uint8_t h[DUNGEON_Y][DUNGEON_X];
	dungeon_path_t path[DUNGEON_Y][DUNGEON_X];

	Pathfinder();
	~Pathfinder();

	int init(uint8_t hardness[DUNGEON_Y][DUNGEON_X]);
	int dijkstra_floor(uint8_t, uint8_t);
	int dijkstra_all(uint8_t, uint8_t);

};

#endif
