#ifndef OBJECTTEMPLATE_H
# define OBJECTTEMPLATE_H

# include <string>
# include <vector>

# include "Dice.h"
# include "Character.h"
# include "NPC.h"

class ObjectTemplate{

public: 

	std::string name;
	std::string description;
	std::string type;
	uint32_t color;
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
	int rarity;

	ObjectTemplate( std::string, // Name
					std::string, // Description
					std::string, // Type
					std::string, // Color
					std::string, // Hit Bonus
					std::string, // Damage Bonus
					std::string, // Dodge Bonus
					std::string, // Defenese Bonus
					std::string, // Weight
					std::string, // Speed Bonus
					std::string, // Attribute
					std::string, // Value
					std::string, // Artifact
					std::string);// Rarity
};

#endif
