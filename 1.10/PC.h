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
# define MAX_SPELLS		 10

typedef struct heap heap_t;

class Dice;
class Object;
class Dungeon;

class PC : public Character{

private:

	uint32_t base_speed = 10;
	int base_hitpoints = 100;

public:

	uint8_t items_carried = 0;

	Object *equipment[EQUIPMENT_SLOTS];
	Object *carry[CARRY_CAPACITY];
	
	const char *spell_names[MAX_SPELLS];
	Dice *spell_dice[MAX_SPELLS];
	uint32_t spell_range[MAX_SPELLS];
	uint8_t num_spells = 0;
	
	PC();
	int init(uint8_t, uint8_t);
	int set_name(char *);
	int update_stats();
	uint32_t attack();
	int pickup(Dungeon &);
	int move(Dungeon &, int c);
	int equip(Dungeon &, char);
	int unequip(Dungeon &, char);
	int drop(Dungeon &, char);
	int expunge(Dungeon &, char);
	int inspect_item(Dungeon &, char);
	int learn_spell(Object &);

};

#endif
