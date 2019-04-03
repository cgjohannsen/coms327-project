#ifndef MONSTERTEMPLATE_H
# define MONSTERTEMPLATE_H

# include <string>
# include <vector>

# include "Dice.h"
# include "Character.h"
# include "NPC.h"

class MonsterTemplate{

public: 

	std::string name;
	std::string description;
	std::vector<Character::Color> color;
	Dice speed;
	std::vector<NPC::Ability> abilities;
	Dice hitpoints;
	Dice attack_damage;
	char symbol;
	int rarity;

	MonsterTemplate(std::string, // Name
					std::string, // Description
					std::string, // Color
					std::string, // Speed
					std::string, // Abilities
					std::string, // hitpoints
					std::string, // attack_damage
					std::string, // symbol
					std::string);// rarity
	~MonsterTemplate();
};

#endif
