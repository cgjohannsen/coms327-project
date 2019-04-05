#include <cstdio>
#include <string>
#include <vector>
#include <sstream>

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
			color.push_back(Character::red);
		} else if(t == "GREEN"){
			color.push_back(Character::green);
		} else if(t == "BLUE"){
			color.push_back(Character::blue);
		} else if(t == "CYAN"){
			color.push_back(Character::cyan);
		} else if(t == "YELLOW"){
			color.push_back(Character::yellow);
		} else if(t == "MAGENTA"){
			color.push_back(Character::magenta);
		} else if(t == "WHITE"){
			color.push_back(Character::white);
		} else if(t == "BLACK"){
			color.push_back(Character::black);
		}
	}

	uint32_t b, d, s;
	sscanf(sp.c_str(), "%d+%dd%d", &b, &d, &s);
	speed.init(b, d, s);

	std::stringstream stream_a(a);
	while(getline(stream_a, t, ' ')){
		if(t == "SMART"){
			abilities.push_back(NPC::smart);
		} else if(t == "TELE"){
			abilities.push_back(NPC::telepathic);
		} else if(t == "TUNNEL"){
			abilities.push_back(NPC::tunneling);
		} else if(t == "ERRATIC"){
			abilities.push_back(NPC::erratic);
		} else if(t == "PASS"){
			abilities.push_back(NPC::pass);
		} else if(t == "PICKUP"){
			abilities.push_back(NPC::pickup);
		} else if(t == "DESTROY"){
			abilities.push_back(NPC::destroy);
		} else if(t == "UNIQ"){
			abilities.push_back(NPC::unique);
		} else if(t == "BOSS"){
			abilities.push_back(NPC::boss);
		}
	}


	sscanf(hp.c_str(), "%d+%dd%d", &b, &d, &s);
	hitpoints.init(b, d, s);

	sscanf(ad.c_str(), "%d+%dd%d", &b, &d, &s);
	attack_damage.init(b, d, s);

	sscanf(sy.c_str(), "%c", &symbol);
	sscanf(r.c_str(), "%d", &rarity);

}
