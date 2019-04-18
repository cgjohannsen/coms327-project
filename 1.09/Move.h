#ifndef MOVE_H
# define MOVE_H

# include "heap.h"

class Character;
class Dungeon;
class NPC;
class PC;

int move_npc(Dungeon &, NPC &);
int move_pc(Dungeon &, heap_t *, int);
int combat(Dungeon &, Character &attacker, Character &defender);

#endif 