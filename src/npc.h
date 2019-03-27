#ifndef NPC_H
# define NPC_H

# include "character.h"

class npc : public character{

private:

	uint16_t characteristics;

public:

	npc(int x, int y, int seed);
	~npc();

	int npc_move00(dungeon &);
	int npc_move01(dungeon &);
	int npc_move02(dungeon &);
	int npc_move03(dungeon &);
	int npc_move04(dungeon &);
	int npc_move05(dungeon &);
	int npc_move06(dungeon &);
	int npc_move07(dungeon &);
	int npc_move08(dungeon &);
	int npc_move09(dungeon &);
	int npc_move10(dungeon &);
	int npc_move11(dungeon &);
	int npc_move12(dungeon &);
	int npc_move13(dungeon &);
	int npc_move14(dungeon &);
	int npc_move15(dungeon &);
	int npc_move(dungeon &);

};

#endif
