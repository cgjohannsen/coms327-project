#include "Object.h"

Object::Object(ObjectTemplate &ob, uint8_t pos_x, uint8_t pos_y)
{
  name = ob.name;
  description = ob.description;
  type = ob.type;
  color = ob.color;
  hit_bonus = ob.hit_bonus.roll();
  damage_bonus = ob.damage_bonus.roll();
  dodge_bonus = ob.dodge_bonus.roll();
  defense_bonus = ob.defense_bonus.roll();
  weight = ob.weight.roll();
  speed_bonus = ob.speed_bonus.roll();
  attribute = ob.attribute.roll();
  value = ob.value.roll();
  
  if(ob.artifact == "TRUE")
    artifact = true;
  else
    artifact = false;

  rarity = ob.rarity;
  symbol = ob.symbol;

  this->x = pos_x;
  this->y = pos_y;
}
