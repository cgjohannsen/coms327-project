#ifndef MONSTERTEMPLATE_H
# define MONSTERTEMPLATE_H

# include <string>
# include <vector>
# include <cstdint>

# include "Dice.h"
# include "Character.h"
# include "NPC.h"

class MonsterTemplate{

public: 

	std::string name;
	std::string description;
	std::vector<uint32_t> color;
	Dice speed;
	uint32_t abilities;
	Dice hitpoints;
	Dice attack_damage;
	char symbol;
	int rarity;
	uint16_t index;
	bool unique = false;
	bool isValid = true;
	uint8_t num_generated = 0;

	MonsterTemplate(std::string, // Name
					std::string, // Description
					std::string, // Color
					std::string, // Speed
					std::string, // Abilities
					std::string, // hitpoints
					std::string, // attack_damage
					std::string, // symbol
					std::string, // rarity
					uint16_t);	 // index
};

#endif
