#ifndef CHARACTER_H
# define CHARACTER_H

# include <cstdint>
# include <string>

# include "heap.h"
# include "Dice.h"

class Dungeon;
class Object;

class Character{

public:

  virtual ~Character();

  uint8_t visual_range;
  uint8_t is_pc;
  heap_node_t *hn;
  uint8_t isAlive;
  uint32_t color;
  uint32_t speed;
  uint32_t move_time;
  int hitpoints;
  Dice attack_damage;
  char symbol;
  uint8_t x;
  uint8_t y;
  uint8_t next_x;
  uint8_t next_y;
  uint16_t template_index = 0;
  std::string name;

  int can_see(Dungeon &, Character &, Character &);
  virtual uint32_t attack();

};

#endif
