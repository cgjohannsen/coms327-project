#include <cstdio>
#include <string>
#include <vector>
#include <sstream>

#include "ObjectTemplate.h"

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

}


