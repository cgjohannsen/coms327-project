#ifndef CHARACTER_H
# define CHARACTER_H

# include "dungeon.h"
# include "heap.h"

#define DUNGEON_Y 21
#define DUNGEON_X 80

# define NPC_SMART   0x00000001 // = b00000001
# define NPC_TELE    0x00000002 // = b00000010
# define NPC_TUNNEL  0x00000004 // = b00000100
# define NPC_ERRATIC 0x00000008 // = b00001000

# define has_characterstic(character, bit) \
     (character.characterstics & NPC_##bit)

typedef struct character {
  heap_node_t *hn;
  uint8_t isPC;
  uint8_t isAlive;
  int id;
  int characteristics;
  uint8_t x;
  uint8_t y;
  uint8_t speed;
  int move_time;
  char symbol;
} character_t;

int character_init_pc(character_t *c, dungeon_t *d);
int character_init_monster(character_t *c, int n, character_t monsters[n],
			   dungeon_t *d, int index);
int character_delete(character_t *c);
int character_render(dungeon_t *d, character_t *pc,
		     int n, character_t monsters[n]);
int character_move(character_t *c, dungeon_t *d, character_t *pc,
		   int n, character_t monsters[n]);
int character_stupid(character_t *c, dungeon_t *d);
int character_smart(character_t *c, dungeon_t *d);
int character_tele(character_t *c, dungeon_t *d);
int character_smart_tele(character_t *c, dungeon_t *d);
int character_tunnel(character_t *c, dungeon_t *d);
int character_tunnel_smart(character_t *c, dungeon_t *d);
int character_tunnel_tele(character_t *c, dungeon_t *d);
int character_tunnel_tele_smart(character_t *c, dungeon_t *d);
int character_erratic(character_t *c, dungeon_t *d);
int character_erratic_smart(character_t *c, dungeon_t *d);
int character_erratic_tele(character_t *c, dungeon_t *d);
int character_erratic_tele_smart(character_t *c, dungeon_t *d);
int character_erratic_tunnel(character_t *c, dungeon_t *d);
int character_erratic_tunnel_smart(character_t *c, dungeon_t *d);
int character_erratic_tunnel_tele(character_t *c, dungeon_t *d);
int character_erratic_tunnel_tele_smart(character_t *c, dungeon_t *d);

#endif
