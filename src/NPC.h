#ifndef NPC_H
# define NPC_H

# include "Character.h"

class NPC : public Character{

private:

	uint16_t characteristics;
	uint8_t last_seen_x;
	uint8_t last_seen_y;

public:

	NPC(int x, int y, int seed);
	~NPC();


	int move(Dungeon &);
	int move00(Dungeon &);
	int move01(Dungeon &);
	int move02(Dungeon &);
	int move03(Dungeon &);
	int move04(Dungeon &);
	int move05(Dungeon &);
	int move06(Dungeon &);
	int move07(Dungeon &);
	int move08(Dungeon &);
	int move09(Dungeon &);
	int move10(Dungeon &);
	int move11(Dungeon &);
	int move12(Dungeon &);
	int move13(Dungeon &);
	int move14(Dungeon &);
	int move15(Dungeon &);

};

#endif
