#include <cstdio>
#include <string>
#include <vector>
#include <sstream>
#include <ncurses.h>

#include "MonsterTemplate.h"

MonsterTemplate::MonsterTemplate(std::string n,  	// Name
								 std::string desc,  // Description
								 std::string c,  	// Color
								 std::string sp,  	// Speed
								 std::string a,  	// Abilities
								 std::string hp, 	// hitpoints
								 std::string ad, 	// attack_damage
								 std::string sy, 	// symbol
								 std::string r)	 	// rarity
{
	name = std::string(n);
	description = std::string(desc);

	std::stringstream stream_c(c);
	std::string t;
	while(getline(stream_c, t, ' ')){
		if(t == "RED"){
			color.push_back(COLOR_RED);
		} else if(t == "GREEN"){
			color.push_back(COLOR_GREEN);
		} else if(t == "BLUE"){
			color.push_back(COLOR_BLUE);
		} else if(t == "CYAN"){
			color.push_back(COLOR_CYAN);
		} else if(t == "YELLOW"){
			color.push_back(COLOR_YELLOW);
		} else if(t == "MAGENTA"){
			color.push_back(COLOR_MAGENTA);
		} else if(t == "WHITE"){
			color.push_back(COLOR_WHITE);
		} else if(t == "BLACK"){
			color.push_back(COLOR_BLACK);
		}
	}

	uint32_t b, d, s;
	sscanf(sp.c_str(), "%d+%dd%d", &b, &d, &s);
	speed.init(b, d, s);

	std::stringstream stream_a(a);
	abilities = 0x0000;
	while(getline(stream_a, t, ' ')){
		if(t == "SMART"){
			abilities += NPC_SMART;
		} else if(t == "TELE"){
			abilities += NPC_TELEPATHIC;
		} else if(t == "TUNNEL"){
			abilities += NPC_TUNNELING;
		} else if(t == "ERRATIC"){
			abilities += NPC_ERRATIC;
		} else if(t == "PASS"){
			abilities += NPC_PASS;
		} else if(t == "PICKUP"){
			abilities += NPC_PICKUP;
		} else if(t == "DESTROY"){
			abilities += NPC_DESTROY;
		} else if(t == "UNIQ"){
			abilities += NPC_UNIQUE;
		} else if(t == "BOSS"){
			abilities += NPC_BOSS;
		}
	}


	sscanf(hp.c_str(), "%d+%dd%d", &b, &d, &s);
	hitpoints.init(b, d, s);

	sscanf(ad.c_str(), "%d+%dd%d", &b, &d, &s);
	attack_damage.init(b, d, s);

	sscanf(sy.c_str(), "%c", &symbol);
	sscanf(r.c_str(), "%d", &rarity);

}
