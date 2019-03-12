#ifndef CHARACTER_H
# define CHARACTER_H

# include <stdint.h>
# include "heap.h"

typedef struct dungeon dungeon_t;
typedef struct npc npc_t;
typedef struct pc pc_t;

typedef struct character {
  heap_node_t *hn;
  pc_t *pc;
  npc_t *npc;
  uint8_t isAlive;
  uint8_t speed;
  uint32_t move_time;
  char symbol;
  uint8_t x;
  uint8_t y;
  uint8_t next_x;
  uint8_t next_y;
} character_t;

void character_delete(void *v);
int can_see(dungeon_t *d, character_t *viewer, character_t *viewee);


#endif
