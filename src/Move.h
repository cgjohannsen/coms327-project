#ifndef MOVE_H
# define MOVE_H

# include "heap.h"
# include <cstdint>

class Character;
class Dungeon;
class NPC;
class PC;

int move_npc(Dungeon &, NPC &);
int move_pc(Dungeon &, heap_t *, int);
int ranged_combat(Dungeon &, uint8_t, uint8_t, uint8_t, uint8_t);
int combat(Dungeon &, Character &attacker, Character &defender, uint32_t damage);

#endif 
