#include "string"

#include "Move.h"

#include "Dungeon.h"
#include "Character.h"
#include "PC.h"
#include "NPC.h"

int move_npc(Dungeon &d, NPC &c)
{
	c.move(d);
	move(d, c);
	return 0;
}

int move_pc(Dungeon &d, int cmd)
{
	int t = d.player.move(d, cmd);
	//move(d, d.player);
	return t;
}

int move(Dungeon &d, Character &c)
{
	if(d.characters[c.next_y][c.next_x] && 
		!(c.next_x == c.x && c.next_y == c.y)){
		combat(c, *d.characters[c.next_y][c.next_x]);
	} else {
		d.characters[c.y][c.x] = NULL;
    	c.x = c.next_x;
    	c.y = c.next_y;
    	d.characters[c.y][c.x] = &c;
	}

	return 0;
}

int combat(Character &attacker, Character &defender)
{
	return 0;
}