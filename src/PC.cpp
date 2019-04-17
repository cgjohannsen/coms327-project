#include <cstdlib>
#include <cstdint>

#include <unistd.h>
#include <ncurses.h>

#include "PC.h"
#include "Dungeon.h"
#include "Dice.h"
#include "IO.h"
#include "Object.h"

PC::PC()
{
  is_pc = 1;
  isAlive = 1;
  color = COLOR_WHITE;
  speed = base_speed;
  move_time = 1000/(speed);
  symbol = '@';
  hitpoints = base_hitpoints;
  attack_damage.init(0, 1, 4);

  uint8_t i;
  for(i = 0; i < CARRY_CAPACITY; i++){
    carry[i] = NULL;
  }
  for(i = 0; i < EQUIPMENT_SLOTS; i++){
    equipment[i] = NULL;
  }
}

int PC::init(uint8_t x_in, uint8_t y_in)
{
  x = x_in;
  y = y_in;
  next_x = x_in;
  next_y = y_in;

  return 0;
}

int PC::set_name(char *s)
{
  name = s;
  return 0;
}

int PC::update_stats()
{
  uint8_t i;
  for(i = 0; i < EQUIPMENT_SLOTS; i++){
    if(equipment[i]){
      speed = base_speed + (*equipment[i]).speed_bonus;
      hitpoints += (*equipment[i]).hit_bonus;
    }
  }

  return 0;
}

uint32_t PC::attack()
{
  uint32_t damage = 0, i;

  damage += attack_damage.roll();

  for(i = 0; i < EQUIPMENT_SLOTS; i++){
    if(equipment[i]){
      damage += (*equipment[i]).damage_bonus.roll();
    }
  }

  return damage;
}

int PC::pickup(Dungeon &d)
{
  uint8_t i;

  if(items_carried < CARRY_CAPACITY) {
    for(i = 0; i < CARRY_CAPACITY; i++){
      if(!carry[i]) {
        carry[i] = d.objects[d.player.y][d.player.x];
        d.objects[d.player.y][d.player.x] = NULL;
        items_carried++;
        break;
      }
    }
  }

  return 0;
}

int PC::equip(Dungeon &d, char c)
{
  int index = c - '0';
  Object *o;

  if(index < CARRY_CAPACITY && !carry[index]) {
    d.message = "There is no item in that slot!";
  } else if(index >= 0 && index < CARRY_CAPACITY && carry[index]) {
    switch(carry[index]->symbol){
      case '|':
      if(!equipment[WEAPON])
        items_carried--;
      o = equipment[WEAPON];
      equipment[WEAPON] = carry[index];
      carry[index] = o;
      break;
      case ')':
      if(!equipment[OFFHAND])
        items_carried--;
      o = equipment[OFFHAND];
      equipment[OFFHAND] = carry[index];
      carry[index] = o;
      break;
      case '}':
      if(!equipment[RANGED])
        items_carried--;
      o = equipment[RANGED];
      equipment[RANGED] = carry[index];
      carry[index] = o;
      break;
      case '[':
      if(!equipment[ARMOR])
        items_carried--;
      o = equipment[ARMOR];
      equipment[ARMOR] = carry[index];
      carry[index] = o;
      break;
      case ']':
      if(!equipment[HELMET])
        items_carried--;
      o = equipment[HELMET];
      equipment[HELMET] = carry[index];
      carry[index] = o;
      break;
      case '(':
      if(!equipment[CLOAK])
        items_carried--;
      o = equipment[CLOAK];
      equipment[CLOAK] = carry[index];
      carry[index] = o;
      break;
      case '{':
      if(!equipment[GLOVES])
        items_carried--;
      o = equipment[GLOVES];
      equipment[GLOVES] = carry[index];
      carry[index] = o;
      break;
      case '\\':
      if(!equipment[BOOTS])
        items_carried--;
      o = equipment[BOOTS];
      equipment[BOOTS] = carry[index];
      carry[index] = o;
      break;
      case '"':
      if(!equipment[AMULET])
        items_carried--;
      o = equipment[AMULET];
      equipment[AMULET] = carry[index];
      carry[index] = o;
      break;
      case '_':
      if(!equipment[LIGHT])
        items_carried--;
      o = equipment[LIGHT];
      equipment[LIGHT] = carry[index];
      carry[index] = o;
      break;
      case '=':
      if(!equipment[RING1] && !equipment[RING2])
        items_carried--;
      if(!equipment[RING1]){
        equipment[RING1] = carry[index];
        carry[index] = NULL;
      } else {
        o = equipment[RING2];
        equipment[RING2] = carry[index];
        carry[index] = o;
      }
      break;
    }
  }

  update_stats();

  return 0;
}

int PC::unequip(Dungeon &d, char c)
{
  int i;

  switch(c){
    case 'a':
    if(equipment[WEAPON] && items_carried < CARRY_CAPACITY){
      for(i = 0; i < CARRY_CAPACITY; i++){
        if(!carry[i]){
          carry[i] = equipment[WEAPON];
          equipment[WEAPON] = NULL;
          items_carried++;
          break;
        }
      }
    } else if(!equipment[WEAPON]) {
      d.message = "Nothing equipped at this spot!";
    } else {
      d.message = "Carry capacity reached: drop something!";
    }
    break;
    case 'b':
    if(equipment[OFFHAND] && items_carried < CARRY_CAPACITY){
      for(i = 0; i < CARRY_CAPACITY; i++){
        if(!carry[i]){
          carry[i] = equipment[OFFHAND];
          equipment[OFFHAND] = NULL;
          items_carried++;
          break;
        }
      }
    } else if(!equipment[OFFHAND]) {
      d.message = "Nothing equipped at this spot!";
    } else {
      d.message = "Carry capacity reached: drop something!";
    }
    break;
    case 'c':
    if(equipment[RANGED] && items_carried < CARRY_CAPACITY){
      for(i = 0; i < CARRY_CAPACITY; i++){
        if(!carry[i]){
          carry[i] = equipment[RANGED];
          equipment[RANGED] = NULL;
          items_carried++;
          break;
        }
      }
    } else if(!equipment[RANGED]) {
      d.message = "Nothing equipped at this spot!";
    } else {
      d.message = "Carry capacity reached: drop something!";
    }
    break;
    case 'd':
    if(equipment[ARMOR] && items_carried < CARRY_CAPACITY){
      for(i = 0; i < CARRY_CAPACITY; i++){
        if(!carry[i]){
          carry[i] = equipment[ARMOR];
          equipment[ARMOR] = NULL;
          items_carried++;
          break;
        }
      }
    } else if(!equipment[ARMOR]) {
      d.message = "Nothing equipped at this spot!";
    } else {
      d.message = "Carry capacity reached: drop something!";
    }
    break;
    case 'e':
    if(equipment[HELMET] && items_carried < CARRY_CAPACITY){
      for(i = 0; i < CARRY_CAPACITY; i++){
        if(!carry[i]){
          carry[i] = equipment[HELMET];
          equipment[HELMET] = NULL;
          items_carried++;
          break;
        }
      }
    } else if(!equipment[HELMET]) {
      d.message = "Nothing equipped at this spot!";
    } else {
      d.message = "Carry capacity reached: drop something!";
    }
    break;
    case 'f':
    if(equipment[CLOAK] && items_carried < CARRY_CAPACITY){
      for(i = 0; i < CARRY_CAPACITY; i++){
        if(!carry[i]){
          carry[i] = equipment[CLOAK];
          equipment[CLOAK] = NULL;
          items_carried++;
          break;
        }
      }
    } else if(!equipment[CLOAK]) {
      d.message = "Nothing equipped at this spot!";
    } else {
      d.message = "Carry capacity reached: drop something!";
    }
    break;
    case 'g':
    if(equipment[GLOVES] && items_carried < CARRY_CAPACITY){
      for(i = 0; i < CARRY_CAPACITY; i++){
        if(!carry[i]){
          carry[i] = equipment[GLOVES];
          equipment[GLOVES] = NULL;
          items_carried++;
          break;
        }
      }
    } else if(!equipment[GLOVES]) {
      d.message = "Nothing equipped at this spot!";
    } else {
      d.message = "Carry capacity reached: drop something!";
    }
    break;
    case 'h':
    if(equipment[BOOTS] && items_carried < CARRY_CAPACITY){
      for(i = 0; i < CARRY_CAPACITY; i++){
        if(!carry[i]){
          carry[i] = equipment[BOOTS];
          equipment[BOOTS] = NULL;
          items_carried++;
          break;
        }
      }
    } else if(!equipment[BOOTS]) {
      d.message = "Nothing equipped at this spot!";
    } else {
      d.message = "Carry capacity reached: drop something!";
    }
    break;
    case 'i':
    if(equipment[AMULET] && items_carried < CARRY_CAPACITY){
      for(i = 0; i < CARRY_CAPACITY; i++){
        if(!carry[i]){
          carry[i] = equipment[AMULET];
          equipment[AMULET] = NULL;
          items_carried++;
          break;
        }
      }
    } else if(!equipment[AMULET]) {
      d.message = "Nothing equipped at this spot!";
    } else {
      d.message = "Carry capacity reached: drop something!";
    }
    break;
    case 'j':
    if(equipment[LIGHT] && items_carried < CARRY_CAPACITY){
      for(i = 0; i < CARRY_CAPACITY; i++){
        if(!carry[i]){
          carry[i] = equipment[LIGHT];
          equipment[LIGHT] = NULL;
          items_carried++;
          break;
        }
      }
    } else if(!equipment[LIGHT]) {
      d.message = "Nothing equipped at this spot!";
    } else {
      d.message = "Carry capacity reached: drop something!";
    }
    break;
    case 'k':
    if(equipment[RING1] && items_carried < CARRY_CAPACITY){
      for(i = 0; i < CARRY_CAPACITY; i++){
        if(!carry[i]){
          carry[i] = equipment[RING1];
          equipment[RING1] = NULL;
          items_carried++;
          break;
        }
      }
    } else if(!equipment[RING1]) {
      d.message = "Nothing equipped at this spot!";
    } else {
      d.message = "Carry capacity reached: drop something!";
    }
    break;
    case 'l':
    if(equipment[RING2] && items_carried < CARRY_CAPACITY){
      for(i = 0; i < CARRY_CAPACITY; i++){
        if(!carry[i]){
          carry[i] = equipment[RING2];
          equipment[RING2] = NULL;
          items_carried++;
          break;
        }
      }
    } else if(!equipment[RING2]) {
      d.message = "Nothing equipped at this spot!";
    } else {
      d.message = "Carry capacity reached: drop something!";
    }
    break;
  }

  update_stats();

  return 0;
}
int PC::drop(Dungeon &d, char c)
{
  int index = c - '0';
  Object *o;

  if(index < CARRY_CAPACITY && !carry[index]) {
    d.message = "There is no item in that slot!";
  } else if(index >= 0 && index < CARRY_CAPACITY && carry[index]) {
    if(!d.objects[d.player.y][d.player.x])
      items_carried--;
    o = d.objects[d.player.y][d.player.x];
    d.objects[d.player.y][d.player.x] = carry[index];
    carry[index] = o;
  }

  return 0;
}
int PC::expunge(Dungeon &d, char c)
{
  int index = c - '0';

  if(index < CARRY_CAPACITY && !carry[index]) {
    d.message = "There is no item in that slot!";
  } else if(index >= 0 && index < CARRY_CAPACITY && carry[index]) {
    delete(carry[index]);
    carry[index] = NULL;
  }

  return 0;
}
int PC::inspect_item(Dungeon &d, char c)
{
  int index = c - '0';

  if(index < CARRY_CAPACITY && !carry[index]) {
    d.message = "There is no item in that slot!";
  } else if(index >= 0 && index < CARRY_CAPACITY && carry[index]) {
    display_object_info(*carry[index]);
  }

  return 0;
}

int PC::move(Dungeon &d, int c)
{
  switch(c) {
    case (int) '7':
    case (int) 'y':
    next_y = y-1;
    next_x = x-1;
    break;
    case (int) '8':
    case (int) 'k':
    next_y = y-1;

    break;
    case (int) '9':
    case (int) 'u':
    next_y = y-1;
    next_x = x+1;
    break;
    case (int) '6':
    case (int) 'l':
    
    next_x = x+1;
    break;
    case (int) '3':
    case (int) 'n':
    next_y = y+1;
    next_x = x+1;
    break;
    case (int) '2':
    case (int) 'j':
    next_y = y+1;

    break;
    case (int) '1':
    case (int) 'b':
    next_y = y+1;
    next_x = x-1;
    break;
    case (int) '4':
    case (int) 'h':
    
    next_x = x-1;
    break;
    case (int) '>':
    return 1;
    case (int) '<':
    return 1;
    case (int) '5':
    case (int) '.':
    case (int) ' ':
    d.message = "The PC bides his (or her) time...";
    break;
    case (int) 'w':
    d.message = "Select item to equip: ";
    print_inventory(d);
    equip(d, getch());
    d.message = "";
    return 2;
    case (int) 't':
    d.message = "Select item to unequip: ";
    print_equipment(d);
    unequip(d, getch());
    d.message = "";
    return 2;
    case (int) 'd':
    d.message = "Select item to drop: ";
    print_inventory(d);
    drop(d, getch());
    d.message = "";
    return 2;
    case (int) 'x':
    d.message = "Select item to expunge from this cursed land: ";
    print_inventory(d);
    expunge(d, getch());
    d.message = "";
    return 2;
    case (int) 'i':
    d.message = "Displaying inventory, press any key to exit";
    print_inventory(d);
    getch();
    d.message = "";
    return 2;
    case (int) 'e':
    d.message = "Displaying equipment, press any key to exit";
    print_equipment(d);
    getch();
    d.message = "";
    return 2;
    case (int) 'I':
    d.message = "Select item to inspect: ";
    print_inventory(d);
    inspect_item(d, getch());
    d.message = "";
    return 2;
    case (int) 'L':
    display_monster_targetting(d);
    return 2;
    case 'm':
    display_monsters(d);
    return 2;
    case 'f':
    d.message = "Revealing dungeon.... (Press f to exit)";
    display_all(d);
    d.message = "";
    return 2;
    case 'g':
    d.message = "Entering teleport mode... (Press t to teleport, r for random)";
    display_teleport(d);
    d.message = "";
    return 2;
    case (int) 'Q':
    return -1;
  }

  return 0;
}