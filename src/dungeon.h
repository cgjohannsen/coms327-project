#ifndef DUNGEON_H
# define DUNGEON_H

# include <stdint.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <endian.h>

# include "heap.h"
# include "character.h"
# include "pathfinder.h"

# define DUNGEON_Y    21
# define DUNGEON_X    80
# define MIN_ROOMS    6
# define MAX_ROOMS    10
# define MAX_UP       3
# define MAX_DOWN     3 
# define ROOM_MIN_X   4
# define ROOM_MIN_Y   3
# define ROOM_MAX_X   20
# define ROOM_MAX_Y   15
# define MAX_BUFFER   1000
# define MAX_MONSTERS 20

typedef enum terrain {
  ter_wall,
  ter_immutable,
  ter_floor,
  ter_corridor,
  ter_stair_up,
  ter_stair_down
} terrain_t;

typedef struct room {
  uint8_t x;
  uint8_t y;
  uint8_t width;
  uint8_t height;
} room_t;

typedef struct stair {
  uint8_t x;
  uint8_t y;
  char dir;
} stair_t;

typedef struct dungeon {
  char *message;
  uint16_t num_rooms;
  room_t *rooms;
  uint16_t num_up;
  uint16_t num_down;
  stair_t *u_stairs;
  stair_t *d_stairs;
  uint16_t nummon;
  character_t pc;
  character_t *characters[DUNGEON_Y][DUNGEON_X];
  dungeon_path_t pc_cost_floor[DUNGEON_Y][DUNGEON_X];
  dungeon_path_t pc_cost_all[DUNGEON_Y][DUNGEON_X];
  uint8_t hardness[DUNGEON_Y][DUNGEON_X];
  terrain_t map[DUNGEON_Y][DUNGEON_X];
  char output[DUNGEON_Y][DUNGEON_X];
} dungeon_t;

int init_dungeon(dungeon_t *d);
int place_rooms(dungeon_t *d);
int place_corridors(dungeon_t *d);
int place_stairs(dungeon_t *d);
int gen_dungeon(dungeon_t *d);
int place_characters(dungeon_t *d);
int update_distances(dungeon_t *d);
int update_output(dungeon_t *d);
int render_pc_cost_floor(dungeon_t *d);
int render_pc_cost_all(dungeon_t *d);
int write_dungeon(dungeon_t *d);
int read_dungeon(dungeon_t *d, uint8_t test, char *n);

#endif
