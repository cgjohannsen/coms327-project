#ifndef OBJECT_H
# define OBJECT_H

# include <string>
# include <cstdint>

# include "Character.h"
# include "ObjectTemplate.h"
# include "Dice.h"

class Object{

public:

	std::string name;
	std::string description;
	std::string type;
	uint32_t color;
    uint32_t hit_bonus;
	Dice damage_bonus;
	uint32_t dodge_bonus;
	uint32_t defense_bonus;
	uint32_t weight;
	uint32_t speed_bonus;
	uint32_t attribute;
	uint32_t value;
    bool artifact;
	char symbol;
	uint32_t rarity;

	uint8_t x;
	uint8_t y;

	Object(ObjectTemplate &, uint8_t, uint8_t);

};

#endif
