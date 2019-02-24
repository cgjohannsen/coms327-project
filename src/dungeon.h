#ifndef DUNGEON_H
# define DUNGEON_H

# include <stdint.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <endian.h>

# include "heap.h"

# define DUNGEON_Y  21
# define DUNGEON_X  80
# define MIN_ROOMS  6
# define MAX_ROOMS  10
# define MAX_UP     3
# define MAX_DOWN   3
# define ROOM_MIN_X 4
# define ROOM_MIN_Y 3
# define ROOM_MAX_X 20
# define ROOM_MAX_Y 15
# define MAX_BUFFER 1000

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

typedef struct pc {
  
} pc_t;

typedef struct npc {
  int characterstics;
} npc_t;

typedef struct character {
  pc_t *pc;
  npc_t *npc;
  uint8_t x;
  uint8_t y;
  uint8_t speed;
  char symbol;
} character_t;

typedef struct dungeon {
  uint16_t num_rooms;
  room_t *rooms;
  stair_t *u_stairs;
  stair_t *d_stairs;
  uint16_t num_up;
  uint16_t num_down;
  character_t pc;
  character_t *characters[DUNGEON_Y][DUNGEON_X];
  uint8_t hardness[DUNGEON_Y][DUNGEON_X];
  char char_arr[DUNGEON_Y][DUNGEON_X];
} dungeon_t;

int init_dungeon(dungeon_t *d);
int place_rooms(dungeon_t *d);
int place_corridors(dungeon_t *d);
int place_stairs(dungeon_t *d);
int gen_dungeon(dungeon_t *d);
int write_char_arr(dungeon_t *d);
int display_dungeon(dungeon_t *d);
int write_dungeon(dungeon_t *d);
int read_dungeon(dungeon_t *d, uint8_t test, char *n);

#endif
