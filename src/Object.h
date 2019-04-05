#ifndef OBJECT_H
# define OBJECT_H

# include <string>
# include <cstdint>

# include "Character.h"
# include "Dice.h"

class Object{

public:

	std::string name;
	std::string description;
	std::string type;
	Character::Color color;
	Dice hit_bonus;
	Dice damage_bonus;
	Dice dodge_bonus;
	Dice defense_bonus;
	Dice weight;
	Dice speed_bonus;
	Dice attribute;
	Dice value;
	std::string artifact;
	char symbol;
	uint32_t rarity;

	Object( std::string,		// Name
			std::string,		// Description
			std::string,		// Type
			Character::Color,	// Color
			uint32_t,			// Hit Bonus
			uint32_t,			// Damage Bonus
			uint32_t,			// Dodge Bonus
			uint32_t,			// Defense Bonus
			uint32_t,			// Weight
			uint32_t,			// Speed Bonus
			uint32_t,			// Attribute
			uint32_t,			// Value
			bool,				// Artifact Flag
			char,				// Symbol
			int);				// Rarity

};

#endif