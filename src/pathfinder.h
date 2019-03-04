#ifndef PATHFINDER_H
# define PATHFINDER_H

# include <stdint.h>
# include "heap.h"

# define DUNGEON_X 80
# define DUNGEON_Y 21

typedef struct dungeon_path{
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} dungeon_path_t;

int pathfinder_init(uint8_t h[DUNGEON_Y][DUNGEON_X],
		    dungeon_path_t path[DUNGEON_Y][DUNGEON_X]);
int pathfinder_dijkstra_floor(uint8_t h[DUNGEON_Y][DUNGEON_X],
			      dungeon_path_t path[DUNGEON_Y][DUNGEON_X],
			      uint8_t pc_x, uint8_t pc_y);
int pathfinder_dijkstra_all(uint8_t h[DUNGEON_Y][DUNGEON_X],
			    dungeon_path_t path[DUNGEON_Y][DUNGEON_X],
			    uint8_t x, uint8_t y);

#endif
