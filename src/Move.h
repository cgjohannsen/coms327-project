#ifndef MOVE_H
# define MOVE_H

class Character;
class Dungeon;
class NPC;
class PC;

int move_npc(Dungeon &, NPC &);
int move_pc(Dungeon &, int);
int move(Dungeon &, Character &);
int combat(Character &attacker, Character &defender);

#endif 