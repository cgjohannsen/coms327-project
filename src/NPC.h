#ifndef NPC_H
# define NPC_H

# include <string>
# include "Character.h"

#define NPC_SMART		0x0001
#define NPC_TELEPATHIC	0x0002
#define NPC_TUNNELING	0x0004
#define NPC_ERRATIC		0x0008
#define NPC_PASS		0x0010
#define NPC_DESTROY		0x0020
#define NPC_PICKUP		0x0040
#define NPC_UNIQUE		0x0080
#define NPC_BOSS		0x0100

class MonsterTemplate;

class NPC : public Character{

public:

	uint8_t last_seen_x;
	uint8_t last_seen_y;
	std::string description;
	uint32_t abilities;
	

	NPC(int, int, int);
	NPC(MonsterTemplate &, int, int);


	int move(Dungeon &);
	int move00(Dungeon &);
	int move01(Dungeon &);
	int move02(Dungeon &);
	int move03(Dungeon &);
	int move04(Dungeon &);
	int move05(Dungeon &);
	int move06(Dungeon &);
	int move07(Dungeon &);

};

#endif
