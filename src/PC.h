#ifndef PC_H
# define PC_H

# include <cstdint>
# include <string>

# include "Character.h"

# define WEAPON  0
# define OFFHAND 1
# define RANGED  2
# define ARMOR   3 
# define HELMET  4
# define CLOAK   5
# define GLOVES  6
# define BOOTS   7 
# define AMULET  8
# define LIGHT   9
# define RING1  10 
# define RING2  11

# define CARRY_CAPACITY  10
# define EQUIPMENT_SLOTS 12

typedef struct heap heap_t;

class Object;
class Dungeon;

class PC : public Character{

public:

	uint8_t items_carried = 0;

	Object *equipment[EQUIPMENT_SLOTS];
	Object *carry[CARRY_CAPACITY];

	PC();
	int init(uint8_t, uint8_t);
	int set_name(char *);
	int pickup(Dungeon &);
	int move(Dungeon &, int c);
	int equip(Dungeon &, char);
	int unequip(Dungeon &, char);
	int drop(Dungeon &, char);
	int expunge(Dungeon &, char);
	int inspect_item(Dungeon &, char);

};

#endif
