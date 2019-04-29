#include <cstdio>
#include <string>
#include <vector>
#include <sstream>
#include <ncurses.h>

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
		color = COLOR_RED;
	} else if(co == "GREEN"){
		color = COLOR_GREEN;
	} else if(co == "BLUE"){
		color = COLOR_BLUE;
	} else if(co == "CYAN"){
		color = COLOR_CYAN;
	} else if(co == "YELLOW"){
		color = COLOR_YELLOW;
	} else if(co == "MAGENTA"){
		color = COLOR_MAGENTA;
	} else if(co == "WHITE"){
		color = COLOR_WHITE;
	} else if(co == "BLACK"){
		color = COLOR_BLACK;
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
	if(!art.compare("TRUE")){
		unique = true;
	}

	sscanf(rrty.c_str(), "%d", &rarity);

	if(!type.compare("WEAPON")){
		symbol = '|';
	} else if(!type.compare("OFFHAND")){
		symbol = ')';
	} else if(!type.compare("RANGED")){
		symbol = '}';
	} else if(!type.compare("ARMOR")){
		symbol = '[';
	} else if(!type.compare("HELMET")){
		symbol = ']';
	} else if(!type.compare("CLOAK")){
		symbol = '(';
	} else if(!type.compare("GLOVES")){
		symbol = '{';
	} else if(!type.compare("BOOTS")){
		symbol = '\\';
	} else if(!type.compare("RING")){
		symbol = '=';
	} else if(!type.compare("AMULET")){
		symbol = '"';
	} else if(!type.compare("LIGHT")){
		symbol = '_';
	} else if(!type.compare("SCROLL")){
		symbol = '~';
	} else if(!type.compare("BOOK")){
		symbol = '?';
	} else if(!type.compare("FLASK")){
		symbol = '!';
	} else if(!type.compare("GOLD")){
		symbol = '$';
	} else if(!type.compare("AUMMUNITION")){
		symbol = '/';
	} else if(!type.compare("FOOD")){
		symbol = ',';
	} else if(!type.compare("WAND")){
		symbol = '-';
	} else if(!type.compare("CONTAINER")){
		symbol = '%';
	}

}


