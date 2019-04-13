#ifndef CHARACTER_H
# define CHARACTER_H

# include <cstdint>

# include "heap.h"
# include "Dice.h"

class Dungeon;
class Object;

class Character{

public:

  uint8_t is_pc;
  heap_node_t *hn;
  uint8_t isAlive;
  uint32_t color;
  uint32_t speed;
  uint32_t move_time;
  uint32_t hitpoints;
  Dice attack_damage;
  Object *object;
  char symbol;
  uint8_t x;
  uint8_t y;
  uint8_t next_x;
  uint8_t next_y;
  uint16_t template_index = 0;

  int can_see(Dungeon &, Character &, Character &);

};

#endif
