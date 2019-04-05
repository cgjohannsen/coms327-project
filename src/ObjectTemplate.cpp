#include <cstdio>
#include <string>
#include <vector>
#include <sstream>

#include "ObjectTemplate.h"
#include "Object.h"

ObjectTemplate::ObjectTemplate( std::string n, // Name
								std::string des, // Description
								std::string ty, // Type
								std::string co, // Color
								std::string hb, // Hit Bonus
								std::string damb, // Damage Bonus
								std::string dogb, // Dodge Bonus
								std::string defb, // Defenese Bonus
								std::string we, // Weight
								std::string sb, // Speed Bonus
								std::string attr, // Attribute
								std::string va, // Value
								std::string art, // Artifact
								std::string rrty) // Rarity
{
	name = std::string(n);
	description = std::string(des);
	type = std::string(ty);

	if(co == "RED"){
		color = Character::red;
	} else if(co == "GREEN"){
		color = Character::green;
	} else if(co == "BLUE"){
		color = Character::blue;
	} else if(co == "CYAN"){
		color = Character::cyan;
	} else if(co == "YELLOW"){
		color = Character::yellow;
	} else if(co == "MAGENTA"){
		color = Character::magenta;
	} else if(co == "WHITE"){
		color = Character::white;
	} else if(co == "BLACK"){
		color = Character::black;
	}

	int b, d, s;
	sscanf(hb.c_str(), "%d+%dd%d", &b, &d, &s);
	hit_bonus.init(b, d, s);

	sscanf(damb.c_str(), "%d+%dd%d", &b, &d, &s);
	damage_bonus.init(b, d, s);

	sscanf(dogb.c_str(), "%d+%dd%d", &b, &d, &s);
	dodge_bonus.init(b, d, s);

	sscanf(defb.c_str(), "%d+%dd%d", &b, &d, &s);
	defense_bonus.init(b, d, s);

	sscanf(we.c_str(), "%d+%dd%d", &b, &d, &s);
	weight.init(b, d, s);

	sscanf(sb.c_str(), "%d+%dd%d", &b, &d, &s);
	speed_bonus.init(b, d, s);

	sscanf(attr.c_str(), "%d+%dd%d", &b, &d, &s);
	attribute.init(b, d, s);

	sscanf(va.c_str(), "%d+%dd%d", &b, &d, &s);
	value.init(b, d, s);

	artifact = std::string(art);

	sscanf(rrty.c_str(), "%d", &rarity);

	if(name == "WEAPON"){
		symbol = '|';
	} else if(name == "OFFHAND"){
		symbol = ')';
	} else if(name == "RANGED"){
		symbol = '}';
	} else if(name == "ARMOR"){
		symbol = '[';
	} else if(name == "HELMET"){
		symbol = ']';
	} else if(name == "CLOAK"){
		symbol = '(';
	} else if(name == "GLOVES"){
		symbol = '{';
	} else if(name == "BOOTS"){
		symbol = '\\';
	} else if(name == "RING"){
		symbol = '=';
	} else if(name == "AMULET"){
		symbol = '"';
	} else if(name == "LIGHT"){
		symbol = '_';
	} else if(name == "SCROLL"){
		symbol = '~';
	} else if(name == "BOOK"){
		symbol = '?';
	} else if(name == "FLASK"){
		symbol = '!';
	} else if(name == "GOLD"){
		symbol = '$';
	} else if(name == "AUMMUNITION"){
		symbol = '/';
	} else if(name == "FOOD"){
		symbol = ',';
	} else if(name == "WAND"){
		symbol = '-';
	} else if(name == "CONTAINER"){
		symbol = '%';
	}

}

Object ObjectTemplate::generate()
{

	bool art;

	if(artifact == "TRUE"){
		art = true;
	} else if(artifact == "FALSE"){
		art = false;
	}

	return Object(name, description, type, color, hit_bonus.roll(), 
		damage_bonus.roll(), dodge_bonus.roll(), defense_bonus.roll(),
		weight.roll(), speed_bonus.roll(), attribute.roll(),
		value.roll(), art, symbol, rarity);
}


